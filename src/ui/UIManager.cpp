#include "ui/UIManager.h"

static bool inRect(int x, int y, const SDL_Rect &r) {
    return (x >= r.x && x < r.x + r.w && y >= r.y && y < r.y + r.h);
}

static SDL_Point toScreenPoint(const SDL_Rect &stage, int x, int y) {
    SDL_Point p;
    int cx = stage.x + stage.w / 2;
    int cy = stage.y + stage.h / 2;
    p.x = cx + x;
    p.y = cy - y;
    return p;
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
    SDL_Point p = toScreenPoint(stage, (int)s.x, (int)s.y);
    SDL_Rect r;
    r.w = 60;
    r.h = 60;
    r.x = p.x - r.w / 2;
    r.y = p.y - r.h / 2;
    return (mx >= r.x && mx < r.x + r.w && my >= r.y && my < r.y + r.h);
}

static void screenToScratch(const SDL_Rect &stage, int mx, int my, int &outX, int &outY) {
    int cx = stage.x + stage.w / 2;
    int cy = stage.y + stage.h / 2;
    outX = mx - cx;
    outY = cy - my;
}

static void normalizeDirection(int &d) {
    d %= 360;
    if (d < 0) d += 360;
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

    if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_t) {
        ui.spriteDraggable = !ui.spriteDraggable;
        ui.draggingSprite = false;
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
            if (!ui.runner.active) {
                buildQueueForEvent(project, GreenFlagClicked, context, ui.runner);
            }
            if (ui.runner.active) {
                stepRunner(context, ui.runner);
            }
            return;
        }

        if (inRect(mx, my, ui.rs.pauseRect)) {
            ui.pausedUI = true;
            pauseExecution(context);
            return;
        }

        if (inRect(mx, my, ui.rs.resumeRect)) {
            ui.pausedUI = false;
            resumeExecution(context);
            return;
        }

        if (ui.spriteDraggable && inSpriteOnScreen(ui.rs.stageRect, context.sprite, mx, my)) {
            ui.draggingSprite = true;
            int sx, sy;
            screenToScratch(ui.rs.stageRect, mx, my, sx, sy);
            ui.dragOffsetX = (int)context.sprite.x - sx;
            ui.dragOffsetY = (int)context.sprite.y - sy;
            return;
        }
    }

    if (e.type == SDL_MOUSEMOTION) {
        if (ui.draggingSprite) {
            int mx = e.motion.x;
            int my = e.motion.y;
            int sx, sy;
            screenToScratch(ui.rs.stageRect, mx, my, sx, sy);
            context.sprite.x = (float)(sx + ui.dragOffsetX);
            context.sprite.y = (float)(sy + ui.dragOffsetY);
            clampSpriteToStage(ui.rs.stageRect, context.sprite);
            return;
        }
    }

    if (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT) {
        ui.draggingSprite = false;
        return;
    }
}