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
#include "ui/Renderer.h"

#include "core/Context.h"

#include <SDL2/SDL.h>

int main() {
    Window w;
    if (!initWindow(w, "Scratch UI - Sadra", 1000, 700)) return 1;

    Context context;      // از core
    RenderState rs;       // برای stage

    // برای تست: جای sprite
    context.sprite.x = 0;
    context.sprite.y = 0;
    context.sprite.visible = true;

    while (w.running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) w.running = false;
        }

        // رندر بر اساس context
        renderAll(w.renderer, rs, context);
    }

    destroyWindow(w);
    return 0;
}