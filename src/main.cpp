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
    Block *def = new Block(DefineFunction);
    def->text = "jump";
    Block *move5 = new Block(Move);
    move5->parameters.push_back(5);
    def->children.push_back(move5);
    Block *call1 = new Block(CallFunction);
    call1->text = "jump";
    Block *call2 = new Block(CallFunction);
    call2->text = "jump";
    script.blocks.push_back(def);
    script.blocks.push_back(call1);
    script.blocks.push_back(call2);
    project.scripts.push_back(script);
    Runner runner;
    cout << "Building queue..." << endl;
    buildQueueForEvent(project, GreenFlagClicked, context, runner);
    cout << "Stepping..." << endl;
    for (int i = 0; i < 10; i++) {
        if (!stepRunner(context, runner)) {
        }
        if (runner.index >= (int) runner.queue.size()) break;
    }
    cout << "Final X = " << context.sprite.x << endl;
    return 0;
}