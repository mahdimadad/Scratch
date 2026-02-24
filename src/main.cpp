#define SDL_MAIN_HANDLED

#include <SDL2/SDL.h>
#include <iostream>

#include "ui/Window.h"
#include "ui/UIManager.h"
#include "ui/Renderer.h"
#include "ui/Text.h"

#include "core/Block.h"
#include "core/Context.h"
#include "core/Events.h"
static void buildDemoProject(Project& project) {
    Script s;
    s.eventType = GreenFlagClicked;

    Block* setv = new Block(SetVariable);
    setv->text = "score";
    setv->parameters.push_back(5);
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

int main(int /*argc*/, char** /*argv*/) {

    Window w;
    if (!initWindow(w, "Scratch UI - Sadra", 1000, 700)) {
        std::cout << "initWindow failed.\n";
        return 1;
    }

    Context context;
    Project project;
    buildDemoProject(project);

    UIManager ui;

    TextSystem ts;
    bool textOk = initText(ts, "arial.ttf", 18);
    if (!textOk) {
        // fallback: فونت ویندوز
        textOk = initText(ts, "C:\\Windows\\Fonts\\arial.ttf", 18);
    }
    if (!textOk) {
        std::cout << "Text init failed (arial.ttf not found).\n";
    }

    SDL_Event e;
    while (w.running) {
        while (SDL_PollEvent(&e)) {
            handleEvent(ui, w, project, context, e);
        }

renderAll(w.renderer, ui.rs, context, ui.pausedUI, ui.runner, ts);    }

    destroyText(ts);
    destroyWindow(w);
    return 0;
}