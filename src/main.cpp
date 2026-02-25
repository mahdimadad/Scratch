#define SDL_MAIN_HANDLED

#include "ui/Window.h"
#include "ui/UIManager.h"
#include "ui/Renderer.h"
#include "ui/Text.h"

#include "core/Block.h"
#include "core/Context.h"
#include "core/Events.h"

#include <SDL2/SDL.h>
#include <string>
#include <iostream>

static void buildDemoProject(Project& project) {
    Script s;
    s.eventType = GreenFlagClicked;

    Block* setv = new Block(SetVariable);
    setv->text = "score";
    setv->parameters.push_back(0);
    s.blocks.push_back(setv);

    for (int i = 0; i < 6; i++) {
        Block* move = new Block(Move);
        move->parameters.push_back(20);
        s.blocks.push_back(move);

        Block* ch = new Block(ChangeVariable);
        ch->text = "score";
        ch->parameters.push_back(1);
        s.blocks.push_back(ch);
    }

    project.scripts.push_back(s);
}

static std::string getFontPath() {
    return "C:/Windows/Fonts/arial.ttf";
}

int main(int argc, char** argv) {
    Window w;
    if (!initWindow(w, "Scratch UI - Sadra", 1100, 720)) return 1;

    Context context;
    Project project;
    buildDemoProject(project);

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
        renderAll(
            w.renderer,
            ui.rs,
            context,
            ui.pausedUI,
            ui.runner,
            ts,
            ui.selectedCategory,
            ui.paletteBlocks,
            ui.workspaceBlocks,
            ui.draggingBlock,
            ui.draggedBlock
        );        SDL_Delay(16);
    }

    destroyText(ts);
    destroyWindow(w);
    return 0;
}