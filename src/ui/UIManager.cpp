#include "ui/UIManager.h"
#include <algorithm>
#include <fstream>
#include <iomanip>

static const char* kSaveFile = "project.ws";

static const int HAT_GREEN = -1001;
static const int HAT_RECEIVE = -1002;

static bool inRect(int x, int y, const SDL_Rect &r) {
    return (x >= r.x && x < r.x + r.w && y >= r.y && y < r.y + r.h);
}

static void screenToScratch(const SDL_Rect &stage, int mx, int my, int &outX, int &outY) {
    int cx = stage.x + stage.w / 2;
    int cy = stage.y + stage.h / 2;
    outX = mx - cx;
    outY = cy - my;
}

static void clampSpriteToStage(const SDL_Rect &stage, SpriteState &s) {
    int halfW = 30;
    int halfH = 30;
    int minX = -(stage.w / 2) + halfW;
    int maxX = (stage.w / 2) - halfW;
    int minY = -(stage.h / 2) + halfH;
    int maxY = (stage.h / 2) - halfH;

    if (s.x < minX) s.x = (float)minX;
    if (s.x > maxX) s.x = (float)maxX;
    if (s.y < minY) s.y = (float)minY;
    if (s.y > maxY) s.y = (float)maxY;
}

static bool inSpriteOnScreen(const SDL_Rect &stage, const SpriteState &s, int mx, int my) {
    if (!s.visible) return false;

    int cx = stage.x + stage.w / 2;
    int cy = stage.y + stage.h / 2;

    int px = cx + (int)s.x;
    int py = cy - (int)s.y;

    SDL_Rect r;
    r.w = 60;
    r.h = 60;
    r.x = px - r.w / 2;
    r.y = py - r.h / 2;

    return inRect(mx, my, r);
}

static void normalizeDirection(int &d) {
    d %= 360;
    if (d < 0) d += 360;
}

static SDL_Rect catRowRect(const RenderState& rs, int idx) {
    int h = 34;
    SDL_Rect row{rs.leftPanelRect.x + 8, rs.leftPanelRect.y + 8 + idx * h, rs.leftPanelRect.w - 16, h - 6};
    return row;
}

static void rebuildProjectFromWorkspace(const std::vector<BlockUI>& ws, Project& project) {
    project.scripts.clear();

    std::vector<BlockUI> ordered = ws;
    std::stable_sort(ordered.begin(), ordered.end(), [](const BlockUI& a, const BlockUI& b) {
        if (a.r.y != b.r.y) return a.r.y < b.r.y;
        return a.r.x < b.r.x;
    });

    Script cur;
    cur.eventType = EVT_GreenFlagClicked;
    cur.messageName = "";

    bool hasHat = false;
    bool hasAnyBlock = false;

    auto flush = [&]() {
        if (!cur.blocks.empty()) {
            project.scripts.push_back(cur);
            cur.blocks.clear();
        }
    };

    for (const auto& uiBlock : ordered) {
        if (uiBlock.coreType == HAT_GREEN) {
            hasHat = true;
            flush();
            cur.eventType = EVT_GreenFlagClicked;
            cur.messageName = "";
            continue;
        }

        if (uiBlock.coreType == HAT_RECEIVE) {
            hasHat = true;
            flush();
            cur.eventType = EVT_MessageReceived;
            cur.messageName = uiBlock.text.empty() ? "msg1" : uiBlock.text;
            continue;
        }

        Block* b = new Block((BlockType)uiBlock.coreType);
        b->parameters = uiBlock.params;
        b->text = uiBlock.text;
        cur.blocks.push_back(b);
        hasAnyBlock = true;
    }

    flush();

    if (!hasHat && hasAnyBlock && project.scripts.empty()) {
        Script s;
        s.eventType = EVT_GreenFlagClicked;
        s.messageName = "";
        project.scripts.push_back(s);
    }
}

static bool saveWorkspaceToFile(const std::vector<BlockUI>& ws, const char* path) {
    std::ofstream out(path, std::ios::binary);
    if (!out.is_open()) return false;

    out << "SCRATCH_WS_1\n";
    out << ws.size() << "\n";

    for (const auto& b : ws) {
        out << b.category << " " << b.coreType << " "
            << b.r.x << " " << b.r.y << " " << b.r.w << " " << b.r.h << " "
            << b.cr << " " << b.cg << " " << b.cb << " "
            << b.params.size() << " ";

        for (size_t i = 0; i < b.params.size(); i++) out << b.params[i] << " ";

        out << std::quoted(b.label) << " " << std::quoted(b.text) << "\n";
    }
    return true;
}

static bool loadWorkspaceFromFile(std::vector<BlockUI>& ws, const char* path) {
    std::ifstream in(path, std::ios::binary);
    if (!in.is_open()) return false;

    std::string header;
    std::getline(in, header);
    if (header != "SCRATCH_WS_1") return false;

    size_t n = 0;
    in >> n;
    ws.clear();
    ws.reserve(n);

    for (size_t i = 0; i < n; i++) {
        BlockUI b;
        size_t pc = 0;
        in >> b.category >> b.coreType
           >> b.r.x >> b.r.y >> b.r.w >> b.r.h
           >> b.cr >> b.cg >> b.cb
           >> pc;

        b.params.clear();
        b.params.reserve(pc);
        for (size_t k = 0; k < pc; k++) {
            int v;
            in >> v;
            b.params.push_back(v);
        }

        in >> std::quoted(b.label) >> std::quoted(b.text);
        ws.push_back(b);
    }
    return true;
}

static int askSaveBeforeClear(SDL_Window* win) {
    const SDL_MessageBoxButtonData buttons[] = {
        { SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 0, "Cancel" },
        { 0, 1, "No" },
        { SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 2, "Save" }
    };
    const SDL_MessageBoxData msgboxdata = {
        SDL_MESSAGEBOX_WARNING,
        win,
        "Unsaved changes",
        "Do you want to save before creating a new project?",
        SDL_arraysize(buttons),
        buttons,
        nullptr
    };
    int buttonid = 0;
    SDL_ShowMessageBox(&msgboxdata, &buttonid);
    return buttonid;
}

void initUIPalette(UIManager& ui) {
    ui.paletteBlocks.clear();

    int x = ui.rs.blockPanelRect.x + 12;
    int y = ui.rs.blockPanelRect.y + 12;
    int w = ui.rs.blockPanelRect.w - 24;
    int h = 46;
    int gap = 12;

    int yy = y;

    auto push = [&](BlockUI b) {
        b.r = SDL_Rect{x, yy, w, h};
        ui.paletteBlocks.push_back(b);
        yy += (h + gap);
    };

    {
        BlockUI e;
        e.category = CAT_EVENTS;
        e.cr = 220; e.cg = 180; e.cb = 30;

        BlockUI e1 = e;
        e1.coreType = HAT_GREEN;
        e1.label = "when green flag clicked";
        e1.text = "";
        push(e1);

        BlockUI r1 = e;
        r1.coreType = HAT_RECEIVE;
        r1.label = "when I receive msg1";
        r1.text = "msg1";
        push(r1);

        BlockUI r2 = e;
        r2.coreType = HAT_RECEIVE;
        r2.label = "when I receive msg2";
        r2.text = "msg2";
        push(r2);

        BlockUI b1 = e;
        b1.coreType = (int)Broadcast;
        b1.label = "broadcast msg1";
        b1.text = "msg1";
        push(b1);

        BlockUI b2 = e;
        b2.coreType = (int)Broadcast;
        b2.label = "broadcast msg2";
        b2.text = "msg2";
        push(b2);

        BlockUI bw1 = e;
        bw1.coreType = (int)BroadcastAndWait;
        bw1.label = "broadcast msg1 and wait";
        bw1.text = "msg1";
        push(bw1);

        BlockUI bw2 = e;
        bw2.coreType = (int)BroadcastAndWait;
        bw2.label = "broadcast msg2 and wait";
        bw2.text = "msg2";
        push(bw2);
    }

    {
        yy = y;

        BlockUI m;
        m.category = CAT_MOTION;
        m.cr = 70; m.cg = 110; m.cb = 230;

        BlockUI mv10 = m;
        mv10.coreType = (int)Move;
        mv10.params = {10};
        mv10.label = "move 10 steps";
        push(mv10);

        BlockUI mv20 = m;
        mv20.coreType = (int)Move;
        mv20.params = {20};
        mv20.label = "move 20 steps";
        push(mv20);

        BlockUI mv_10 = m;
        mv_10.coreType = (int)Move;
        mv_10.params = {-10};
        mv_10.label = "move -10 steps";
        push(mv_10);

        BlockUI tr15 = m;
        tr15.coreType = (int)Turn;
        tr15.params = {15};
        tr15.label = "turn right 15";
        push(tr15);

        BlockUI tr30 = m;
        tr30.coreType = (int)Turn;
        tr30.params = {30};
        tr30.label = "turn right 30";
        push(tr30);

        BlockUI tl15 = m;
        tl15.coreType = (int)Turn;
        tl15.params = {-15};
        tl15.label = "turn left 15";
        push(tl15);

        BlockUI tl30 = m;
        tl30.coreType = (int)Turn;
        tl30.params = {-30};
        tl30.label = "turn left 30";
        push(tl30);

        BlockUI w1 = m;
        w1.coreType = (int)Wait;
        w1.params = {1};
        w1.label = "wait 1 seconds";
        push(w1);

        BlockUI w2 = m;
        w2.coreType = (int)Wait;
        w2.params = {2};
        w2.label = "wait 2 seconds";
        push(w2);

        BlockUI w3 = m;
        w3.coreType = (int)Wait;
        w3.params = {3};
        w3.label = "wait 3 seconds";
        push(w3);
    }

    {
        yy = y;

        BlockUI v;
        v.category = CAT_VARIABLES;
        v.cr = 240; v.cg = 150; v.cb = 40;

        BlockUI s0 = v;
        s0.coreType = (int)SetVariable;
        s0.text = "score";
        s0.params = {0};
        s0.label = "set score to 0";
        push(s0);

        BlockUI s10 = v;
        s10.coreType = (int)SetVariable;
        s10.text = "score";
        s10.params = {10};
        s10.label = "set score to 10";
        push(s10);

        BlockUI s100 = v;
        s100.coreType = (int)SetVariable;
        s100.text = "score";
        s100.params = {100};
        s100.label = "set score to 100";
        push(s100);

        BlockUI c1 = v;
        c1.coreType = (int)ChangeVariable;
        c1.text = "score";
        c1.params = {1};
        c1.label = "change score by 1";
        push(c1);

        BlockUI c5 = v;
        c5.coreType = (int)ChangeVariable;
        c5.text = "score";
        c5.params = {5};
        c5.label = "change score by 5";
        push(c5);

        BlockUI c_1 = v;
        c_1.coreType = (int)ChangeVariable;
        c_1.text = "score";
        c_1.params = {-1};
        c_1.label = "change score by -1";
        push(c_1);

        BlockUI c_5 = v;
        c_5.coreType = (int)ChangeVariable;
        c_5.text = "score";
        c_5.params = {-5};
        c_5.label = "change score by -5";
        push(c_5);
    }
}

void handleEvent(UIManager &ui, Window &w, Project &project, Context &context, const SDL_Event &e) {

    if (e.type == SDL_QUIT) {
        w.running = false;
        return;
    }

    if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
        w.running = false;
        return;
    }

    if (e.type == SDL_KEYDOWN && (e.key.keysym.mod & KMOD_CTRL) && e.key.keysym.sym == SDLK_s) {
        saveWorkspaceToFile(ui.workspaceBlocks, kSaveFile);
        return;
    }

    if (e.type == SDL_KEYDOWN && (e.key.keysym.mod & KMOD_CTRL) && e.key.keysym.sym == SDLK_o) {
        ui.runner.active = false;
        ui.pausedUI = false;
        context.isRunning = false;
        loadWorkspaceFromFile(ui.workspaceBlocks, kSaveFile);
        return;
    }

    if (e.type == SDL_KEYDOWN && (e.key.keysym.mod & KMOD_CTRL) && e.key.keysym.sym == SDLK_n) {
        if (!ui.workspaceBlocks.empty()) {
            int r = askSaveBeforeClear(w.window);
            if (r == 0) return;
            if (r == 2) saveWorkspaceToFile(ui.workspaceBlocks, kSaveFile);
        }
        ui.runner.active = false;
        ui.pausedUI = false;
        context.isRunning = false;
        ui.workspaceBlocks.clear();
        project.scripts.clear();
        return;
    }

    if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_a) {
        context.sprite.direction -= 10;
        normalizeDirection(context.sprite.direction);
        return;
    }

    if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_d) {
        context.sprite.direction += 10;
        normalizeDirection(context.sprite.direction);
        return;
    }

    if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_UP) {
        context.sprite.y += 10;
        clampSpriteToStage(ui.rs.stageRect, context.sprite);
        return;
    }

    if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_DOWN) {
        context.sprite.y -= 10;
        clampSpriteToStage(ui.rs.stageRect, context.sprite);
        return;
    }

    if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_LEFT) {
        context.sprite.x -= 10;
        clampSpriteToStage(ui.rs.stageRect, context.sprite);
        return;
    }

    if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RIGHT) {
        context.sprite.x += 10;
        clampSpriteToStage(ui.rs.stageRect, context.sprite);
        return;
    }

    if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
        int mx = e.button.x;
        int my = e.button.y;

        if (inRect(mx, my, ui.rs.newRect)) {
            if (!ui.workspaceBlocks.empty()) {
                int r = askSaveBeforeClear(w.window);
                if (r == 0) return;
                if (r == 2) saveWorkspaceToFile(ui.workspaceBlocks, kSaveFile);
            }
            ui.runner.active = false;
            ui.pausedUI = false;
            context.isRunning = false;
            ui.workspaceBlocks.clear();
            project.scripts.clear();
            return;
        }

        if (inRect(mx, my, ui.rs.saveRect)) {
            saveWorkspaceToFile(ui.workspaceBlocks, kSaveFile);
            return;
        }

        if (inRect(mx, my, ui.rs.loadRect)) {
            ui.runner.active = false;
            ui.pausedUI = false;
            context.isRunning = false;
            loadWorkspaceFromFile(ui.workspaceBlocks, kSaveFile);
            return;
        }

        if (inRect(mx, my, ui.rs.greenFlagRect)) {
            ui.runner.active = false;
            ui.pausedUI = false;
            context.isRunning = true;
            rebuildProjectFromWorkspace(ui.workspaceBlocks, project);
            buildQueueForEvent(project, EVT_GreenFlagClicked, context, ui.runner);
            return;
        }

        if (inRect(mx, my, ui.rs.stepRect)) {
            if (!ui.runner.active) {
                context.isRunning = true;
                rebuildProjectFromWorkspace(ui.workspaceBlocks, project);
                buildQueueForEvent(project, EVT_GreenFlagClicked, context, ui.runner);
            }
            stepRunner(context, ui.runner);
            if (isRunnerDone(ui.runner) || !context.isRunning) ui.runner.active = false;
            return;
        }

        if (inRect(mx, my, ui.rs.pauseRect)) {
            ui.pausedUI = true;
            return;
        }

        if (inRect(mx, my, ui.rs.resumeRect)) {
            ui.pausedUI = false;
            return;
        }

        for (int i = 0; i < 5; i++) {
            if (inRect(mx, my, catRowRect(ui.rs, i))) {
                ui.selectedCategory = i;
                return;
            }
        }

        for (auto& b : ui.paletteBlocks) {
            if (b.category == ui.selectedCategory && inRect(mx, my, b.r)) {
                ui.draggingBlock = true;
                ui.draggedBlock = b;
                ui.dragBlockOffX = mx - b.r.x;
                ui.dragBlockOffY = my - b.r.y;
                return;
            }
        }

        if (inSpriteOnScreen(ui.rs.stageRect, context.sprite, mx, my)) {
            ui.draggingSprite = true;
            int sx, sy;
            screenToScratch(ui.rs.stageRect, mx, my, sx, sy);
            ui.dragOffsetX = (int)context.sprite.x - sx;
            ui.dragOffsetY = (int)context.sprite.y - sy;
            return;
        }
    }

    if (e.type == SDL_MOUSEMOTION) {
        if (ui.draggingBlock) {
            ui.draggedBlock.r.x = e.motion.x - ui.dragBlockOffX;
            ui.draggedBlock.r.y = e.motion.y - ui.dragBlockOffY;
            return;
        }

        if (ui.draggingSprite) {
            int sx, sy;
            screenToScratch(ui.rs.stageRect, e.motion.x, e.motion.y, sx, sy);
            context.sprite.x = (float)(sx + ui.dragOffsetX);
            context.sprite.y = (float)(sy + ui.dragOffsetY);
            clampSpriteToStage(ui.rs.stageRect, context.sprite);
            return;
        }
    }

    if (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT) {
        if (ui.draggingBlock) {
            if (inRect(e.button.x, e.button.y, ui.rs.stageRect))
                ui.workspaceBlocks.push_back(ui.draggedBlock);

            ui.draggingBlock = false;
            return;
        }

        ui.draggingSprite = false;
    }
}