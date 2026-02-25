#include "core/Context.h"
#include "core/Block.h"
#include "core/Events.h"
#include "core/MultiRunner.h"
#include <iostream>
using namespace std;
int main() {
    Context context;
    context.isRunning = true;
    context.sprite.costumes = {"c1", "c2", "c3"};
    context.backdrops = {"b1", "b2"};
    Project project;
    Script s;
    s.eventType = EVT_GreenFlagClicked;
    Block *hide = new Block(Hide);
    Block *setSize = new Block(SetSize); setSize->parameters.push_back(150);
    Block *nextC = new Block(NextCostume);
    Block *setC = new Block(SetCostume); setC->text = "c3";
    Block *nextB = new Block(NextBackdrop);
    Block *setB = new Block(SetBackdrop); setB->text = "b2";
    Block *clr = new Block(ClearEffects);
    Block *chg = new Block(ChangeColorEffect); chg->parameters.push_back(7);
    Block *front = new Block(GoToFrontLayer);

    s.blocks = {hide, setSize, nextC, setC, nextB, setB, clr, chg, front};
    project.scripts.push_back(s);

    MultiRunner mr;
    buildMultiForEvent(project, EVT_GreenFlagClicked, context, mr);

    int safety = 0;
    while (safety++ < 200 && stepMulti(context, project, mr)) {}

    cout << "visible=" << context.sprite.visible << "\n";
    cout << "size=" << context.sprite.sizePercent << "\n";
    cout << "costumeIndex=" << context.sprite.costumeIndex << "\n";
    cout << "backdropIndex=" << context.backdropIndex << "\n";
    cout << "colorEffect=" << context.sprite.colorEffect << "\n";
    cout << "layer=" << context.sprite.layer << "\n";
    return 0;
}