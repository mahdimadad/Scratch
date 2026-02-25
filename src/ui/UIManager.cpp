#include "ui/UIManager.h"
#include <algorithm>
#include <fstream>
#include <iomanip>

static const char* kSaveFile = "project.ws";

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

static void rebuildGreenFlagScriptFromWorkspace(const std::vector<BlockUI>& ws, Project& project) {
    project.scripts.clear();

    Script s;
    s.eventType = EVT_GreenFlagClicked;

    std::vector<BlockUI> ordered = ws;
    std::stable_sort(ordered.begin(), ordered.end(), [](const BlockUI& a, const BlockUI& b) {
        if (a.r.y != b.r.y) return a.r.y < b.r.y;
        return a.r.x < b.r.x;
    });

    for (const auto& uiBlock : ordered) {
        Block* b = new Block((BlockType)uiBlock.coreType);
        b->parameters = uiBlock.params;
        b->text = uiBlock.text;
        s.blocks.push_back(b);
    }

    project.scripts.push_back(s);
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

    BlockUI b1;
    b1.category = CAT_MOTION;
    b1.coreType = (int)Move;
    b1.params = {10};
    b1.label = "move 10 steps";
    b1.cr = 70; b1.cg = 110; b1.cb = 230;
    b1.r = SDL_Rect{x, y, w, h};
    ui.paletteBlocks.push_back(b1);

    BlockUI b2 = b1;
    b2.coreType = (int)Turn;
    b2.params = {15};
    b2.label = "turn right 15";
    b2.r = SDL_Rect{x, y + (h + gap), w, h};
    ui.paletteBlocks.push_back(b2);

    BlockUI b3 = b1;
    b3.coreType = (int)Turn;
    b3.params = {-15};
    b3.label = "turn left 15";
    b3.r = SDL_Rect{x, y + 2 * (h + gap), w, h};
    ui.paletteBlocks.push_back(b3);

    BlockUI b4 = b1;
    b4.coreType = (int)Wait;
    b4.params = {1};
    b4.label = "wait 1 seconds";
    b4.r = SDL_Rect{x, y + 3 * (h + gap), w, h};
    ui.paletteBlocks.push_back(b4);

    BlockUI v1;
    v1.category = CAT_VARIABLES;
    v1.coreType = (int)SetVariable;
    v1.text = "score";
    v1.params = {0};
    v1.label = "set score to 0";
    v1.cr = 240; v1.cg = 150; v1.cb = 40;
    v1.r = SDL_Rect{x, y + 4 * (h + gap), w, h};
    ui.paletteBlocks.push_back(v1);

    BlockUI v2 = v1;
    v2.coreType = (int)ChangeVariable;
    v2.text = "score";
    v2.params = {1};
    v2.label = "change score by 1";
    v2.r = SDL_Rect{x, y + 5 * (h + gap), w, h};
    ui.paletteBlocks.push_back(v2);
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
            rebuildGreenFlagScriptFromWorkspace(ui.workspaceBlocks, project);
            buildQueueForEvent(project, EVT_GreenFlagClicked, context, ui.runner);
            return;
        }

        if (inRect(mx, my, ui.rs.stepRect)) {
            if (!ui.runner.active) {
                context.isRunning = true;
                rebuildGreenFlagScriptFromWorkspace(ui.workspaceBlocks, project);
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