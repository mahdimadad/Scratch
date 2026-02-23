#include "ui/UIManager.h"
static bool inRect(int x, int y,const SDL_Rect &r) {
    return (x>=r.x && x<r.x+r.w &&y>=r.y && y<r.y+r.h);
}
void handleEvent(UIManager& ui,Window& w, Project& project,Context& context, const SDL_Event& e) {
    if (e.type == SDL_QUIT) {
        w.running = false;
        return;
    }
    if (e.type == SDL_KEYDOWN&& e.key.keysym.sym == SDLK_ESCAPE) {
        w.running = false;
        return;
    }
    if (e.type == SDL_MOUSEBUTTONDOWN&&e.button.button == SDL_BUTTON_LEFT) {
        int mx=e.button.x;
        int my=e.button.y;
        if (inRect(mx,my,ui.rs.greenFlagRect)) {
            ui.runner.active = false;
            runEvent(project,GreenFlagClicked,context);
            return;
        }

        if (inRect(mx,my,ui.rs.stepRect)) {
            if (!ui.runner.active) {
                buildQueueForEvent(project,GreenFlagClicked,context,ui.runner);
            }
            if (ui.runner.active) {
                stepRunner(context,ui.runner);
            }
            return;
        }
        if (inRect(mx,my,ui.rs.pauseRect)) {
            ui.pausedUI=true;
            pauseExecution(context);
            return;
        }
        if (inRect(mx,my,ui.rs.resumeRect)) {
            ui.pausedUI=false;
            resumeExecution(context);
            return;
        }
    }
}