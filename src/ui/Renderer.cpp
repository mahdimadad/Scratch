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

static void fillRect(SDL_Renderer* ren, const SDL_Rect& r, int rr, int gg, int bb) {
    SDL_SetRenderDrawColor(ren, rr, gg, bb, 255);
    SDL_RenderFillRect(ren, &r);
}

static void strokeRect(SDL_Renderer* ren, const SDL_Rect& r, int rr, int gg, int bb) {
    SDL_SetRenderDrawColor(ren, rr, gg, bb, 255);
    SDL_RenderDrawRect(ren, &r);
}

static void drawButton(SDL_Renderer *ren, const SDL_Rect &r, int rr, int gg, int bb) {
    fillRect(ren, r, rr, gg, bb);
    strokeRect(ren, r, 255, 255, 255);
}

static float deg2rad(float d) {
    return d * 3.14159265f / 180.0f;
}

static void drawDirectionLine(SDL_Renderer *ren, const SDL_Rect &stage, const SpriteState &s) {
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
    fillRect(ren, r, 220, 180, 40);
    strokeRect(ren, r, 0, 0, 0);
    drawDirectionLine(ren, stage, s);
}

static void drawPanel(SDL_Renderer* ren, const SDL_Rect& r) {
    fillRect(ren, r, 35, 35, 40);
    strokeRect(ren, r, 120, 120, 120);
}

static const char* catName(int c) {
    if (c == CAT_MOTION) return "Motion";
    if (c == CAT_LOOKS) return "Looks";
    if (c == CAT_EVENTS) return "Events";
    if (c == CAT_CONTROL) return "Control";
    return "Variables";
}

static void drawCategories(SDL_Renderer* ren, TextSystem& text, const RenderState& rs, int selected) {
    int x = rs.leftPanelRect.x + 12;
    int y = rs.leftPanelRect.y + 12;
    int h = 34;

    for (int i = 0; i < 5; i++) {
        SDL_Rect row{rs.leftPanelRect.x + 8, rs.leftPanelRect.y + 8 + i * h, rs.leftPanelRect.w - 16, h - 6};
        if (i == selected) fillRect(ren, row, 60, 60, 70);
        else fillRect(ren, row, 45, 45, 55);
        strokeRect(ren, row, 110, 110, 110);
        drawText(ren, text, x, y + i * h, catName(i));
    }
}

static void drawBlock(SDL_Renderer* ren, TextSystem& text, const BlockUI& b) {
    fillRect(ren, b.r, b.cr, b.cg, b.cb);
    strokeRect(ren, b.r, 255, 255, 255);
    drawText(ren, text, b.r.x + 10, b.r.y + 10, b.label);
}

void renderAll(SDL_Renderer *ren,
               const RenderState &rs,
               const Context &context,
               bool pausedUI,
               const Runner &runner,
               TextSystem &text,
               int selectedCategory,
               const std::vector<BlockUI>& paletteBlocks,
               const std::vector<BlockUI>& workspaceBlocks,
               bool draggingBlock,
               const BlockUI& draggedBlock) {
    SDL_SetRenderDrawColor(ren, 25, 25, 30, 255);
    SDL_RenderClear(ren);

    drawButton(ren, rs.greenFlagRect, 40, 160, 60);
    drawButton(ren, rs.stepRect, 80, 80, 200);
    drawButton(ren, rs.pauseRect, 200, 80, 80);
    drawButton(ren, rs.resumeRect, 80, 200, 120);
    drawButton(ren, rs.clearRect, 120, 120, 120);

    drawText(ren, text, rs.greenFlagRect.x + 18, rs.greenFlagRect.y + 10, "Run");
    drawText(ren, text, rs.stepRect.x + 22, rs.stepRect.y + 10, "Step");
    drawText(ren, text, rs.pauseRect.x + 18, rs.pauseRect.y + 10, "Pause");
    drawText(ren, text, rs.resumeRect.x + 12, rs.resumeRect.y + 10, "Resume");
    drawText(ren, text, rs.clearRect.x + 18, rs.clearRect.y + 10, "Clear");

    drawPanel(ren, rs.leftPanelRect);
    drawPanel(ren, rs.blockPanelRect);

    if (pausedUI) fillRect(ren, rs.stageRect, 35, 35, 35);
    else fillRect(ren, rs.stageRect, 50, 50, 60);
    strokeRect(ren, rs.stageRect, 200, 200, 200);

    drawCategories(ren, text, rs, selectedCategory);

    for (const auto& b : paletteBlocks) {
        if (b.category == selectedCategory) drawBlock(ren, text, b);
    }

    for (const auto& b : workspaceBlocks) drawBlock(ren, text, b);

    drawSprite(ren, rs.stageRect, context.sprite);

    int hx = rs.stageRect.x + 10;
    int hy = rs.stageRect.y + 10;
    int lineH = 24;

    drawText(ren, text, hx, hy, "Runner: " + std::string(runner.active ? "ACTIVE" : "IDLE"));
    drawText(ren, text, hx, hy + lineH, "Queue: " + std::to_string((int)runner.queue.size()));
    drawText(ren, text, hx, hy + lineH * 2, "Index: " + std::to_string((int)runner.index));
    drawText(ren, text, hx, hy + lineH * 3, "Paused: " + std::string(pausedUI ? "YES" : "NO"));
    drawText(ren, text, hx, hy + lineH * 4, "dir = " + std::to_string((int)context.sprite.direction));

    if (draggingBlock) drawBlock(ren, text, draggedBlock);

    SDL_RenderPresent(ren);
}