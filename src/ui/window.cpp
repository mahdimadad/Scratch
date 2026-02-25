#include "ui/window.h"
#include <iostream>

bool initWindow(Window& w,const char* title,int width,int height) {
    w.width=width;
    w.height=height;

    if (SDL_Init(SDL_INIT_VIDEO)!=0) {
        std::cout<<"SDL_Init failed: "<<SDL_GetError()<<std::endl;
        return false;
    }
w.window=SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w.width, w.height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);    if (!w.window) {
        std::cout<<"Create window failed: "<<SDL_GetError()<<std::endl;
        return false;
    }
    w.renderer=SDL_CreateRenderer(w.window, -1,SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
    if (!w.renderer) {
        std::cout<<"Create renderer failed: "<<SDL_GetError()<<std::endl;
        return false;
    }
    w.running=true;
    return true;
}
void destroyWindow(Window& w) {
    if (w.renderer) {
        SDL_DestroyRenderer(w.renderer);
    }
    if (w.window) {
        SDL_DestroyWindow(w.window);
    }
    w.renderer=nullptr;
    w.window=nullptr;
    SDL_Quit();
}