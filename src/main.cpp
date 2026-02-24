#include "core/Block.h"
#include "core/Context.h"
#include "core/Events.h"
#include "core/MultiRunner.h"
#include <iostream>
using namespace std;
int main() {
    Context context;
    Project project;
    Script scriptA;
    scriptA.eventType = EVT_GreenFlagClicked;
    Block *bw = new Block(BroadcastAndWait);
    bw->text = "go";
    Block *moveAfter = new Block(Move);
    moveAfter->parameters.push_back(5);
    scriptA.blocks.push_back(bw);
    scriptA.blocks.push_back(moveAfter);
    Script scriptB;
    scriptB.eventType = EVT_MessageReceived;
    scriptB.messageName = "go";
    Block *moveB = new Block(Move);
    moveB->parameters.push_back(10);
    scriptB.blocks.push_back(moveB);
    project.scripts.push_back(scriptA);
    project.scripts.push_back(scriptB);
    MultiRunner mr;
    buildMultiForEvent(project, EVT_GreenFlagClicked, context, mr);
    while (!isMultiDone(mr)) { stepMulti(context, project, mr); }
    cout << "Final X = " << context.sprite.x << endl;
    return 0;
}