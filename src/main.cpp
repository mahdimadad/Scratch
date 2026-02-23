#include "core/Block.h"
#include "core/Context.h"
#include "core/Events.h"
#include "core/Runner.h"
#include <iostream>
#include <chrono>
using namespace std;
static unsigned long long nowMs() {
    using namespace std::chrono;
    return (unsigned long long) duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
}
int main() {
    Context context;
    Project project;
    Script script;
    script.eventType = GreenFlagClicked;
    Block *move1 = new Block(Move);
    move1->parameters.push_back(10);
    Block *waitBlock = new Block(Wait);
    waitBlock->parameters.push_back(2);
    Block *move2 = new Block(Move);
    move2->parameters.push_back(10);
    script.blocks.push_back(move1);
    script.blocks.push_back(waitBlock);
    script.blocks.push_back(move2);
    project.scripts.push_back(script);
    Runner runner;
    buildQueueForEvent(project, GreenFlagClicked, context, runner);
    cout << "Starting execution..." << endl;
    unsigned long long start = nowMs();
    while (!isRunnerDone(runner)) {
        if (stepRunner(context, runner)) {
            cout << "Time: " << (nowMs() - start) / 1000.0 << "s | X = " << context.sprite.x << endl;
        }
    }
    cout << "\nDONE" << endl;
    cout << "Final X = " << context.sprite.x << endl;
    return 0;
}