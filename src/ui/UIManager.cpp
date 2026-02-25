#include "ui/UIManager.h"

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

void initUIPalette(UIManager& ui) {
    ui.paletteBlocks.clear();

    int x = ui.rs.blockPanelRect.x + 12;
    int y = ui.rs.blockPanelRect.y + 12;
    int w = ui.rs.blockPanelRect.w - 24;
    int h = 46;
    int gap = 12;

    BlockUI b1;
    b1.category = CAT_MOTION;
    b1.label = "move 10 steps";
    b1.cr = 70; b1.cg = 110; b1.cb = 230;
    b1.r = SDL_Rect{x, y, w, h};
    ui.paletteBlocks.push_back(b1);

    BlockUI b2 = b1;
    b2.label = "turn right 15";
    b2.r = SDL_Rect{x, y + (h + gap), w, h};
    ui.paletteBlocks.push_back(b2);

    BlockUI b3 = b1;
    b3.label = "turn left 15";
    b3.r = SDL_Rect{x, y + 2 * (h + gap), w, h};
    ui.paletteBlocks.push_back(b3);
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

        if (inRect(mx, my, ui.rs.greenFlagRect)) {
            ui.runner.active = false;
            runEvent(project, GreenFlagClicked, context);
            return;
        }

        if (inRect(mx, my, ui.rs.stepRect)) {
            if (!ui.runner.active)
                buildQueueForEvent(project, GreenFlagClicked, context, ui.runner);

            if (ui.runner.active)
                stepRunner(context, ui.runner);

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