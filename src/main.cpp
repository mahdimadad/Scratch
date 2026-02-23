#include "core/Block.h"
#include "core/Context.h"
#include "core/Engine.h"
#include "core/Events.h"
#include <iostream>
using namespace std;
int main() {
    Context context;
    Project project;
    Script script;
    script.eventType = GreenFlagClicked;
    Block *move = new Block(Move);
    move->parameters.push_back(10);
    Block *repeat = new Block(Repeat);
    repeat->parameters.push_back(3);
    repeat->children.push_back(move);
    script.blocks.push_back(repeat);
    project.scripts.push_back(script);
    cout << "=== TEST 1: Pause before execution ===" << endl;
    pauseExecution(context);
    runEvent(project, GreenFlagClicked, context);
    cout << "X after pause = " << context.sprite.x << endl;
    cout << "\n=== TEST 2: Resume and execute ===" << endl;
    resumeExecution(context);
    runEvent(project, GreenFlagClicked, context);
    cout << "Final X = " << context.sprite.x << endl;
    return 0;
}