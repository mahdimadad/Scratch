#ifndef UI_UIMANAGER_H
#define UI_UIMANAGER_H

#include <SDL2/SDL.h>
#include <vector>
#include <string>
#include "core/Engine.h"
#include "core/Runner.h"

#include "ui/UIData.h"
#include "ui/Window.h"

#include "core/Context.h"
#include "core/Events.h"
#include "core/Runner.h"

struct UIManager {
    RenderState rs;
    Runner runner;
    bool pausedUI = false;

    bool spriteDraggable = true;
    bool draggingSprite = false;
    int dragOffsetX = 0;
    int dragOffsetY = 0;

    int selectedCategory = CAT_MOTION;

    std::vector<BlockUI> paletteBlocks;
    std::vector<BlockUI> workspaceBlocks;

    bool draggingBlock = false;
    BlockUI draggedBlock;
    int dragBlockOffX = 0;
    int dragBlockOffY = 0;
};

void initUIPalette(UIManager& ui);
void handleEvent(UIManager& ui, Window& w, Project& project, Context& context, const SDL_Event& e);

#endif