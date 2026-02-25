#include "ui/Renderer.h"
#include <cmath>

static SDL_Point toScreen(const SDL_Rect &stage, int x, int y) {
    SDL_Point p;
    int cx = stage.x + stage.w / 2;
    int cy = stage.y + stage.h / 2;
    p.x = cx + x;
    p.y = cy - y;
    return p;
}

static void drawButton(SDL_Renderer *ren, const SDL_Rect &r, int rr, int gg, int bb) {
    SDL_SetRenderDrawColor(ren, rr, gg, bb, 255);
    SDL_RenderFillRect(ren, &r);
    SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
    SDL_RenderDrawRect(ren, &r);
}

static void drawStage(SDL_Renderer *ren, const SDL_Rect &stage, bool pausedUI) {
    if (pausedUI) SDL_SetRenderDrawColor(ren, 35, 35, 35, 255);
    else SDL_SetRenderDrawColor(ren, 50, 50, 60, 255);
    SDL_RenderFillRect(ren, &stage);
    SDL_SetRenderDrawColor(ren, 200, 200, 200, 255);
    SDL_RenderDrawRect(ren, &stage);
}

static float deg2rad(float d) {
    return d * 3.14159265f / 180.0f;
}

static void drawDirectionLine(SDL_Renderer *ren, const SDL_Rect &stage, const SpriteState &s) {
    if (!s.visible) return;

    SDL_Point c = toScreen(stage, (int)s.x, (int)s.y);

    float a = deg2rad((float)s.direction);
    float dx = cosf(a);
    float dy = -sinf(a);

    int len = 45;
    int x2 = c.x + (int)(dx * len);
    int y2 = c.y + (int)(dy * len);

    SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
    SDL_RenderDrawLine(ren, c.x, c.y, x2, y2);
}

static void drawSprite(SDL_Renderer *ren, const SDL_Rect &stage, const SpriteState &s) {
    if (!s.visible) return;
    SDL_Point p = toScreen(stage, (int)s.x, (int)s.y);
    SDL_Rect r;
    r.w = 60;
    r.h = 60;
    r.x = p.x - r.w / 2;
    r.y = p.y - r.h / 2;
    SDL_SetRenderDrawColor(ren, 220, 180, 40, 255);
    SDL_RenderFillRect(ren, &r);
    SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
    SDL_RenderDrawRect(ren, &r);
}

void renderAll(SDL_Renderer *ren,
               const RenderState &rs,
               const Context &context,
               bool pausedUI,
               const Runner &runner,
               TextSystem &text) {
    SDL_SetRenderDrawColor(ren, 25, 25, 30, 255);
    SDL_RenderClear(ren);

    drawButton(ren, rs.greenFlagRect, 40, 160, 60);
    drawButton(ren, rs.stepRect, 80, 80, 200);
    drawButton(ren, rs.pauseRect, 200, 80, 80);
    drawButton(ren, rs.resumeRect, 80, 200, 120);

    drawStage(ren, rs.stageRect, pausedUI);
    drawSprite(ren, rs.stageRect, context.sprite);
    drawDirectionLine(ren, rs.stageRect, context.sprite);

    int x = rs.hudRect.x;
    int y = rs.hudRect.y;
    int lineH = 28;

    drawText(ren, text, x, y, "Runner: " + std::string(runner.active ? "ACTIVE" : "IDLE"));
    drawText(ren, text, x, y + lineH, "Queue: " + std::to_string((int)runner.queue.size()));
    drawText(ren, text, x, y + lineH * 2, "Index: " + std::to_string((int)runner.index));
    drawText(ren, text, x, y + lineH * 3, "Paused: " + std::string(pausedUI ? "YES" : "NO"));

    auto it = context.variables.find("score");
    if (it != context.variables.end()) {
        drawText(ren, text, x, y + lineH * 4, "score = " + std::to_string(it->second));
    }

    drawText(ren, text, x, y + lineH * 5, "dir = " + std::to_string((int)context.sprite.direction));

    SDL_RenderPresent(ren);
}