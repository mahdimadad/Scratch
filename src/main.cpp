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
    Block *setv = new Block(SetVariable);
    setv->text = "score";
    setv->parameters.push_back(10);
    Block *chv = new Block(ChangeVariable);
    chv->text = "score";
    chv->parameters.push_back(5);
    script.blocks.push_back(repeat);
    script.blocks.push_back(setv);
    script.blocks.push_back(chv);
    project.scripts.push_back(script);
    cout << "Green flag clicked!" << endl;
    runEvent(project, GreenFlagClicked, context);
    cout << "Final X = " << context.sprite.x << endl;
    cout << "score = " << context.variables["score"] << endl;
    return 0;
}