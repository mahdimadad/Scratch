#include "core/Block.h"
#include "core/Context.h"
#include "core/Events.h"
#include "core/Runner.h"
#include <iostream>
using namespace std;
int main() {
    Context context;
    Project project;
    Script script;
    script.eventType = GreenFlagClicked;
    Block *foreverBlock = new Block(Forever);
    Block *move = new Block(Move);
    move->parameters.push_back(1);
    foreverBlock->children.push_back(move);
    script.blocks.push_back(foreverBlock);
    project.scripts.push_back(script);
    Runner runner;
    buildQueueForEvent(project, GreenFlagClicked, context, runner);
    cout << "Testing Forever block...\n" << endl;
    for (int i = 0; i < 20; i++) {
        stepRunner(context, runner);
        cout << "Step " << i + 1 << " | X = " << context.sprite.x << endl;
    }
    cout << "\nDONE" << endl;
    return 0;
}