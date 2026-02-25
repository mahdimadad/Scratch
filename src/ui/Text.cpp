#include "ui/Text.h"
#include <SDL2/SDL_ttf.h>
#include <iostream>

bool initText(TextSystem& ts,const char* fontPath, int fontSize) {
    if (TTF_Init()!=0) {
        std::cout<<"TTF_Init failed: "<<TTF_GetError()<<std::endl;
        ts.ready=false;
        return false;
    }

    ts.font=TTF_OpenFont(fontPath,fontSize);
    if (!ts.font) {
        std::cout<<"TTF_OpenFont failed: ("<<fontPath<<") "<<TTF_GetError()<<std::endl;
        ts.ready=false;
        return false;
    }
    ts.ready=true;
    return true;
}

void destroyText(TextSystem& ts) {
    if (ts.font) {
        TTF_CloseFont(ts.font);
        ts.font=nullptr;
    }
    TTF_Quit();
    ts.ready=false;
}

void drawTextColored(SDL_Renderer* ren, TextSystem& ts,int x, int y, const std::string& text, SDL_Color color) {
    if (!ts.ready||!ts.font) {
        return;
    }
    SDL_Surface* surf=TTF_RenderUTF8_Blended(ts.font,text.c_str(),color);
    if (!surf) {
        return;
    }
    SDL_Texture* tex=SDL_CreateTextureFromSurface(ren,surf);
    if (!tex) {
        SDL_FreeSurface(surf);
        return;
    }

    SDL_Rect dst;
    dst.x=x;
    dst.y=y;
    dst.w=surf->w;
    dst.h=surf->h;
    SDL_FreeSurface(surf);
    SDL_RenderCopy(ren,tex,nullptr,&dst);
    SDL_DestroyTexture(tex);
}

void drawText(SDL_Renderer* ren, TextSystem& ts,int x, int y, const std::string& text) {
    SDL_Color color{255,255,255,255};
    drawTextColored(ren, ts, x, y, text, color);
}