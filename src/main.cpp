#include "core/Block.h"
#include "core/Context.h"
#include "core/Events.h"
#include "core/Runner.h"
#include <iostream>
using namespace std;
int main() {
    Context context;
    Project project;
    Script s;
    s.eventType = GreenFlagClicked;
    Block *move = new Block(Move);
    move->parameters.push_back(10);
    Block *rep = new Block(Repeat);
    rep->parameters.push_back(3);
    rep->children.push_back(move);
    Block *setv = new Block(SetVariable);
    setv->text = "score";
    setv->parameters.push_back(10);
    Block *chv = new Block(ChangeVariable);
    chv->text = "score";
    chv->parameters.push_back(5);
    s.blocks.push_back(rep);
    s.blocks.push_back(setv);
    s.blocks.push_back(chv);
    project.scripts.push_back(s);
    Runner runner;
    cout << "Building queue..." << endl;
    buildQueueForEvent(project, GreenFlagClicked, context, runner);
    cout << "Queue size = " << runner.queue.size() << endl;
    cout << "Stepping..." << endl;
    while (stepRunner(context, runner)) {
        cout << "Step done. X=" << context.sprite.x << " score=" << context.variables["score"] << endl;
    }
    cout << "DONE." << endl;
    cout << "Final X = " << context.sprite.x << endl;
    cout << "Final score = " << context.variables["score"] << endl;
    return 0;
}