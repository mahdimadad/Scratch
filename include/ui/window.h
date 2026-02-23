#ifndef UI_WINDOW_H
#define UI_WINDOW_H

#include <SDL2/SDL.h>
struct Window {
    SDL_Window *window=nullptr;
    SDL_Renderer *renderer=nullptr;
    int width=1000;
    int height=700;
    bool running=true;
};
bool initWindow(Window& w,const char* title,int width,int height);
void destroyWindow(Window& w);
#endif