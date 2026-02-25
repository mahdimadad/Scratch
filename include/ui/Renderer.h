#ifndef UI_RENDERER_H
#define UI_RENDERER_H

#include <SDL2/SDL.h>
#include <vector>
#include <string>

#include "ui/UIData.h"
#include "ui/Text.h"

#include "core/Context.h"
#include "core/Runner.h"

void renderAll(SDL_Renderer* ren,
               const RenderState& rs,
               const Context& context,
               bool pausedUI,
               const Runner& runner,
               TextSystem& text,
               int selectedCategory,
               const std::vector<BlockUI>& paletteBlocks,
               const std::vector<BlockUI>& workspaceBlocks,
               bool draggingBlock,
               const BlockUI& draggedBlock);

#endif