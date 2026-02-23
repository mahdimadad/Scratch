// #include "core/Block.h"
// #include "core/Context.h"
// #include "core/Events.h"
// #include "core/Runner.h"
// #include <iostream>
// using namespace std;
// int main() {
//     Context context;
//     Project project;
//     Script script;
//     script.eventType = GreenFlagClicked;
//     Block *setv = new Block(SetVariable);
//     setv->text = "score";
//     setv->parameters.push_back(12);
//     Block *cond = new Block(If);
//     cond->text = "score";
//     cond->parameters.push_back(1);
//     cond->parameters.push_back(10);
//     Block *move = new Block(Move);
//     move->parameters.push_back(10);
//     cond->children.push_back(move);
//     script.blocks.push_back(setv);
//     script.blocks.push_back(cond);
//     project.scripts.push_back(script);
//     Runner runner;
//     cout << "Building queue..." << endl;
//     buildQueueForEvent(project, GreenFlagClicked, context, runner);
//     cout << "Queue size = " << runner.queue.size() << endl;
//     cout << "Stepping..." << endl;
//     while (stepRunner(context, runner)) {
//         cout << "Step executed -> X = " << context.sprite.x << " | score = " << context.variables["score"] << endl;
//     }
//     cout << "\nDONE" << endl;
//     cout << "Final X = " << context.sprite.x << endl;
//     cout << "Final score = " << context.variables["score"] << endl;
//     return 0;
// }

#define SDL_MAIN_HANDLED

#include "ui/Window.h"
#include "ui/UIManager.h"
#include "ui/Renderer.h"

#include "core/Block.h"
#include "core/Context.h"
#include "core/Events.h"

#include <SDL2/SDL.h>

static void buildDemoProject(Project& project) {
    Script s;
    s.eventType = GreenFlagClicked;
    for (int i=0; i<6; i++) {
        Block* move = new Block(Move);
    move->parameters.push_back(20);
        s.blocks.push_back(move);
    }
    project.scripts.push_back(s);
}

int main() {
    Window w;
    if (!initWindow(w, "Scratch UI - Sadra", 1000, 700)) return 1;

    Context context;
    Project project;
    buildDemoProject(project);

    UIManager ui;

    SDL_Event e;
    while (w.running) {
        while (SDL_PollEvent(&e)) {
            handleEvent(ui, w, project, context, e);
        }
        renderAll(w.renderer, ui.rs, context);
    }

    destroyWindow(w);
    return 0;
}