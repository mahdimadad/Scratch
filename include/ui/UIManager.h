#ifndef UI_UIMANAGER_H
#define UI_UIMANAGER_H
#include <SDL2/SDL.h>
#include "ui/window.h"
#include "ui/Renderer.h"

#include "core/Context.h"
#include "core/Events.h"
#include "core/Runner.h"
#include "core/Engine.h"

struct UIManager {
    RenderState rs;
    Runner runner;
    bool pausedUI=false;
};

void handleEvent(UIManager& ui,Window& w, Project& project,Context& context, const SDL_Event& e);
#endif