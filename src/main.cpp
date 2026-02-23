#include "core/Block.h"
#include "core/Context.h"
#include "core/Events.h"
#include "core/Runner.h"
#include <iostream>
using namespace std;
int main() {
    Context context;
    Project project;
    context.sprite.direction = 90;
    Script script;
    script.eventType = GreenFlagClicked;
    Block *setv = new Block(SetVariable);
    setv->text = "score";
    setv->parameters.push_back(12);
    Block *cond = new Block(If);
    cond->text = "score";
    cond->parameters.push_back(1);
    cond->parameters.push_back(10);
    Block *move = new Block(Move);
    move->parameters.push_back(10);
    cond->children.push_back(move);
    script.blocks.push_back(setv);
    script.blocks.push_back(cond);
    project.scripts.push_back(script);
    Runner runner;
    cout << "Building queue..." << endl;
    buildQueueForEvent(project, GreenFlagClicked, context, runner);
    cout << "Queue size = " << runner.queue.size() << endl;
    cout << "Stepping..." << endl;
    while (stepRunner(context, runner)) {
        cout << "Step -> X=" << context.sprite.x << " Y=" << context.sprite.y << " dir=" << context.sprite.direction <<
                " score=" << context.variables["score"] << endl;
    }
    cout << "\nDONE" << endl;
    cout << "Final X = " << context.sprite.x << endl;
    cout << "Final Y = " << context.sprite.y << endl;
    cout << "Final score = " << context.variables["score"] << endl;
    return 0;
}