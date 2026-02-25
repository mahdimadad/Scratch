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

static int clampi(int v, int lo, int hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

static void drawSprite(SDL_Renderer *ren, const SDL_Rect &stage, const SpriteState &s) {
    if (!s.visible) return;
    SDL_Point p = toScreen(stage, (int)s.x, (int)s.y);

    SDL_Rect r;
    int base = 60;
    int sz = (int)std::round(base * (s.sizePercent / 100.0f));
    sz = clampi(sz, 6, 300);

    r.w = sz;
    r.h = sz;
    r.x = p.x - r.w / 2;
    r.y = p.y - r.h / 2;

    int rr = clampi(220 + (s.colorEffect % 128), 0, 255);
    int gg = clampi(180 - (s.colorEffect % 128), 0, 255);
    int bb = clampi(40 + (s.colorEffect % 64), 0, 255);

    fillRect(ren, r, rr, gg, bb);
    strokeRect(ren, r, 0, 0, 0);
    drawDirectionLine(ren, stage, s);
}

static void drawTextShadow(SDL_Renderer* ren, TextSystem& text, int x, int y, const std::string& s) {
    drawText(ren, text, x + 1, y + 1, s);
    drawText(ren, text, x, y, s);
}

static void drawBubble(SDL_Renderer *ren, TextSystem &text, const SDL_Rect &stage, const SpriteState &s) {
    if (s.bubbleText.empty()) return;

    SDL_Point p = toScreen(stage, (int)s.x, (int)s.y);

    int paddingX = 10;
    int paddingY = 8;
    int w = 260;
    int h = 48;

    SDL_Rect b;
    b.w = w;
    b.h = h;
    b.x = p.x - w / 2;
    b.y = p.y - 60 - h - 12;

    if (b.y < stage.y + 6) b.y = p.y + 60;

    int br, bg, bb;
    int rr, rg, rb;

    if (s.bubbleIsThink) {
        br = 225; bg = 235; bb = 255;
        rr = 45;  rg = 70;  rb = 140;
    } else {
        br = 255; bg = 255; bb = 255;
        rr = 60;  rg = 60;  rb = 60;
    }

    fillRect(ren, b, br, bg, bb);
    strokeRect(ren, b, rr, rg, rb);

    int tx = b.x + paddingX;
    int ty = b.y + paddingY;

    SDL_Rect inner = SDL_Rect{b.x + 6, b.y + 6, b.w - 12, b.h - 12};
    SDL_RenderSetClipRect(ren, &inner);

    SDL_Color shadow{255,255,255,255};
    SDL_Color fg{0,0,0,255};

    drawTextColored(ren, text, tx + 1, ty + 1, s.bubbleText, shadow);
    drawTextColored(ren, text, tx, ty, s.bubbleText, fg);

    SDL_RenderSetClipRect(ren, nullptr);
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

static void catColor(int c, int& r, int& g, int& b) {
    if (c == CAT_MOTION) { r=70; g=110; b=230; return; }
    if (c == CAT_LOOKS) { r=140; g=90; b=210; return; }
    if (c == CAT_EVENTS) { r=220; g=180; b=30; return; }
    if (c == CAT_CONTROL) { r=170; g=90; b=220; return; }
    r=240; g=150; b=40;
}

static void drawCategories(SDL_Renderer* ren, TextSystem& text, const RenderState& rs, int selected) {
    int x = rs.leftPanelRect.x + 12;
    int y = rs.leftPanelRect.y + 12;
    int h = 34;

    for (int i = 0; i < 5; i++) {
        SDL_Rect row{rs.leftPanelRect.x + 8, rs.leftPanelRect.y + 8 + i * h, rs.leftPanelRect.w - 16, h - 6};

        int r,g,b;
        catColor(i, r, g, b);

        if (i == selected) fillRect(ren, row, r, g, b);
        else fillRect(ren, row, (r*70)/100, (g*70)/100, (b*70)/100);

        strokeRect(ren, row, 255, 255, 255);

        int lum = (int)(0.299*r + 0.587*g + 0.114*b);
        SDL_Color fg = (lum > 140) ? SDL_Color{0,0,0,255} : SDL_Color{255,255,255,255};

        drawTextColored(ren, text, x, y + i * h, catName(i), fg);
    }
}

static void drawBlock(SDL_Renderer* ren, TextSystem& text, const BlockUI& b) {
    fillRect(ren, b.r, b.cr, b.cg, b.cb);
    strokeRect(ren, b.r, 255, 255, 255);
    drawText(ren, text, b.r.x + 10, b.r.y + 10, b.label);
}

static int getVar(const Context& ctx, const std::string& name, int defVal) {
    auto it = ctx.variables.find(name);
    if (it == ctx.variables.end()) return defVal;
    return it->second;
}

void renderAll(SDL_Renderer* ren,
               const RenderState& rs,
               const Context& context,
               bool pausedUI,
               const Runner& runner,
               TextSystem& text,
               int selectedCategory,
               int paletteScrollY,
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

    drawButton(ren, rs.newRect, 120, 120, 120);
    drawButton(ren, rs.saveRect, 120, 120, 120);
    drawButton(ren, rs.loadRect, 120, 120, 120);

    drawText(ren, text, rs.greenFlagRect.x + 18, rs.greenFlagRect.y + 10, "Run");
    drawText(ren, text, rs.stepRect.x + 22, rs.stepRect.y + 10, "Step");
    drawText(ren, text, rs.pauseRect.x + 18, rs.pauseRect.y + 10, "Pause");
    drawText(ren, text, rs.resumeRect.x + 12, rs.resumeRect.y + 10, "Resume");

    drawText(ren, text, rs.newRect.x + 26, rs.newRect.y + 10, "New");
    drawText(ren, text, rs.saveRect.x + 22, rs.saveRect.y + 10, "Save");
    drawText(ren, text, rs.loadRect.x + 22, rs.loadRect.y + 10, "Load");

    drawPanel(ren, rs.leftPanelRect);
    drawPanel(ren, rs.blockPanelRect);

    if (pausedUI) fillRect(ren, rs.stageRect, 35, 35, 35);
    else fillRect(ren, rs.stageRect, 50, 50, 60);
    strokeRect(ren, rs.stageRect, 200, 200, 200);

    drawCategories(ren, text, rs, selectedCategory);

    SDL_RenderSetClipRect(ren, &rs.blockPanelRect);

    for (const auto& b : paletteBlocks) {
        if (b.category != selectedCategory) continue;
        BlockUI bb = b;
        bb.r.y += paletteScrollY;
        if (bb.r.y + bb.r.h < rs.blockPanelRect.y) continue;
        if (bb.r.y > rs.blockPanelRect.y + rs.blockPanelRect.h) continue;
        drawBlock(ren, text, bb);
    }

    SDL_RenderSetClipRect(ren, nullptr);

    for (const auto& b : workspaceBlocks) drawBlock(ren, text, b);

    drawSprite(ren, rs.stageRect, context.sprite);
drawBubble(ren, text, rs.stageRect, context.sprite);    int hx = rs.stageRect.x + 10;
    int hy = rs.stageRect.y + 10;
    int lineH = 24;

    drawText(ren, text, hx, hy, "Runner: " + std::string(runner.active ? "ACTIVE" : "IDLE"));
    drawText(ren, text, hx, hy + lineH, "Queue: " + std::to_string((int)runner.queue.size()));
    drawText(ren, text, hx, hy + lineH * 2, "Index: " + std::to_string((int)runner.index));
    drawText(ren, text, hx, hy + lineH * 3, "Paused: " + std::string(pausedUI ? "YES" : "NO"));
    drawText(ren, text, hx, hy + lineH * 4, "dir = " + std::to_string((int)context.sprite.direction));

    int score = getVar(context, "score", 0);
    drawText(ren, text, hx, hy + lineH * 5, "score = " + std::to_string(score));

    if (draggingBlock) drawBlock(ren, text, draggedBlock);

    SDL_RenderPresent(ren);
}