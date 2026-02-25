#define SDL_MAIN_HANDLED

#include "ui/Window.h"
#include "ui/UIManager.h"
#include "ui/Renderer.h"
#include "ui/Text.h"

#include "core/Context.h"
#include "core/Runner.h"

#include <SDL2/SDL.h>
#include <string>
#include <iostream>

static std::string getFontPath() {
    return "C:/Windows/Fonts/arial.ttf";
}

int main(int argc, char** argv) {
    Window w;
    if (!initWindow(w, "Scratch UI - Sadra", 1100, 720)) return 1;

    Context context;
    Project project;

    UIManager ui;
    initUIPalette(ui);

    TextSystem ts;
    std::string fontPath = getFontPath();
    if (!initText(ts, fontPath.c_str(), 24)) {
        std::cout << "Text init failed! Check arial.ttf next to exe.\n";
    }

    SDL_Event e;
    while (w.running) {
        while (SDL_PollEvent(&e)) {
            handleEvent(ui, w, project, context, e);
        }

        if (ui.runner.active && !ui.pausedUI && context.isRunning) {
            stepRunner(context, ui.runner);
            if (isRunnerDone(ui.runner) || !context.isRunning) ui.runner.active = false;
        }

        renderAll(
     w.renderer,
     ui.rs,
     context,
     ui.pausedUI,
     ui.runner,
     ts,
     ui.selectedCategory,
     ui.paletteScroll[ui.selectedCategory],
     ui.paletteBlocks,
     ui.workspaceBlocks,
     ui.draggingBlock,
     ui.draggedBlock
 );

        SDL_Delay(16);
    }

    destroyText(ts);
    destroyWindow(w);
    return 0;
}