#ifndef UI_RENDERER_H
#define UI_RENDERER_H
#include <SDL2/SDL.h>
#include "core/Context.h"

struct RenderState {
    SDL_Rect stageRect{200,80,760,580};
};
void renderAll(SDL_Renderer* ren, const RenderState& rs, const Context& context);
#endif