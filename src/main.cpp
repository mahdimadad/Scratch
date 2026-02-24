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

// ======================= Demo Project =======================
static void buildDemoProject(Project& project) {
    Script s;
    s.eventType = GreenFlagClicked;

    // Demo ساده برای اینکه Step و Run واضح باشه
    for (int i = 0; i < 6; i++) {
        Block* move = new Block(Move);
        move->parameters.push_back(20);
        s.blocks.push_back(move);
    }

    project.scripts.push_back(s);
}

// ======================= Entry Point =======================
int main(int /*argc*/, char** /*argv*/) {
    // 1) Window init
    Window w;
    if (!initWindow(w, "Scratch UI - Sadra", 1000, 700)) {
        std::cout << "initWindow failed.\n";
        return 1;
    }

    // 2) Core state
    Context context;
    Project project;
    buildDemoProject(project);

    // 3) UI
    UIManager ui;

    // 4) Text (SDL_ttf)
    TextSystem ts;
    bool textOk = initText(ts, "arial.ttf", 18); // کنار exe
    if (!textOk) {
        // fallback: فونت ویندوز
        textOk = initText(ts, "C:\\Windows\\Fonts\\arial.ttf", 18);
    }
    if (!textOk) {
        std::cout << "Text init failed (arial.ttf not found).\n";
        // ادامه می‌دیم حتی بدون متن
    }

    // 5) Main loop
    SDL_Event e;
    while (w.running) {
        while (SDL_PollEvent(&e)) {
            handleEvent(ui, w, project, context, e);
        }

        // رندر (حتماً امضای renderAll شما باید این باشه)
        // renderAll(renderer, rs, context, pausedUI, ts)
        renderAll(w.renderer, ui.rs, context, ui.pausedUI, ts);
    }

    // 6) Cleanup
    destroyText(ts);
    destroyWindow(w);
    return 0;
}