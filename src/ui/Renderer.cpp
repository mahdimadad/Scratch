#include "ui/Renderer.h"
static SDL_Point toScreen(const SDL_Rect& stage, int x, int y) {
    SDL_Point p;
    int cx= stage.x+stage.w/2;
    int cy= stage.y+stage.h/2;

    p.x=cx+x;
    p.y=cy-y;
    return p;
}
static void drawStage(SDL_Renderer* ren, const SDL_Rect& stage) {
    SDL_SetRenderDrawColor(ren,50,50,60,255);
    SDL_RenderFillRect(ren,&stage);
    SDL_SetRenderDrawColor(ren,200,200,200,255);
    SDL_RenderDrawRect(ren,&stage);
}
static void drawButton(SDL_Renderer* ren, const SDL_Rect& r,int rr, int gg, int bb) {
    SDL_SetRenderDrawColor(ren,rr,gg,bb,255);
    SDL_RenderFillRect(ren,&r);
    SDL_SetRenderDrawColor(ren,255,255,255,255);
    SDL_RenderDrawRect(ren,&r);
}
static void drawSprite(SDL_Renderer* ren, const SDL_Rect& stage, const SpriteState& s) {
    if (!s.visible)return;
    SDL_Point p = toScreen(stage, s.x, s.y);
    SDL_Rect r;
    r.w=60;
    r.h=60;
    r.x=p.x-r.w/2;
    r.y=p.y-r.h/2;

    SDL_SetRenderDrawColor(ren,220,180,40,255);
    SDL_RenderFillRect(ren,&r);

    SDL_SetRenderDrawColor(ren,0,0,0,255);
    SDL_RenderDrawRect(ren,&r);
}
void renderAll(SDL_Renderer* ren, const RenderState& rs,const Context& context,bool pausedUI) {
    SDL_SetRenderDrawColor(ren,25,25,30,255);
    SDL_RenderClear(ren);
    drawButton(ren,rs.greenFlagRect,40,160,60);
    drawButton(ren,rs.stepRect,80,80,200);
    drawButton(ren,rs.pauseRect,200,80,80);
    drawButton(ren,rs.resumeRect,80,200,120);

    if (pausedUI) {
        SDL_SetRenderDrawColor(ren,35,35,35,255);
    }
    else {
        SDL_SetRenderDrawColor(ren,50,50,50,255);
    }
    SDL_RenderFillRect(ren,&rs.stageRect);

    SDL_SetRenderDrawColor(ren,200,200,200,255);
    SDL_RenderDrawRect(ren,&rs.stageRect);

    drawSprite(ren,rs.stageRect,context.sprite);

    SDL_RenderPresent(ren);

}