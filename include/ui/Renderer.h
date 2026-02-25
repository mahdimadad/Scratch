#ifndef UI_RENDERER_H
#define UI_RENDERER_H

#include <SDL2/SDL.h>
#include <string>
#include "core/Context.h"
#include "core/Runner.h"
#include "ui/Text.h"

struct RenderState {
    SDL_Rect greenFlagRect{20, 20, 120, 40};
    SDL_Rect stepRect{150, 20, 90, 40};
    SDL_Rect pauseRect{250, 20, 90, 40};
    SDL_Rect resumeRect{350, 20, 90, 40};
    SDL_Rect stageRect{200, 80, 760, 580};
    SDL_Rect hudRect{20, 80, 170, 300};
};

void renderAll(SDL_Renderer* ren,
               const RenderState& rs,
               const Context& context,
               bool pausedUI,
               const Runner& runner,
               TextSystem& text);

#endif