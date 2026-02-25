#ifndef UI_UIDATA_H
#define UI_UIDATA_H

#include <SDL2/SDL.h>
#include <string>
#include <vector>
#include "core/Block.h"

struct RenderState {
    SDL_Rect greenFlagRect{20, 20, 120, 40};
    SDL_Rect stepRect{150, 20, 90, 40};
    SDL_Rect pauseRect{250, 20, 90, 40};
    SDL_Rect resumeRect{350, 20, 90, 40};

    SDL_Rect newRect{450, 20, 90, 40};
    SDL_Rect saveRect{550, 20, 90, 40};
    SDL_Rect loadRect{650, 20, 90, 40};

    SDL_Rect leftPanelRect{20, 80, 160, 580};
    SDL_Rect blockPanelRect{190, 80, 240, 580};
    SDL_Rect stageRect{450, 80, 530, 580};

    SDL_Rect hudRect{20, 160, 160, 500};
};

enum UICategory {
    CAT_MOTION = 0,
    CAT_LOOKS = 1,
    CAT_EVENTS = 2,
    CAT_CONTROL = 3,
    CAT_VARIABLES = 4
};

struct BlockUI {
    SDL_Rect r{0, 0, 0, 0};
    int category = CAT_MOTION;

    int coreType = (int)Move;
    std::vector<int> params;
    std::string text;

    std::string label = "block";
    int cr = 80, cg = 80, cb = 200;
};

#endif