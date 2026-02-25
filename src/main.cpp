#include "core/Context.h"
#include "core/Block.h"
#include "core/Events.h"
#include "core/MultiRunner.h"
#include <iostream>
using namespace std;
int main() {
    Context context;
    context.isRunning = true;
    Project project;
    Script s;
    s.eventType = EVT_GreenFlagClicked;
    Block *setv = new Block(SetVariable);
    setv->text = "score";
    setv->parameters.push_back(5);
    Block *gt = new Block(Greater);
    Block *lhs = new Block(VariableValue); lhs->text = "score";
    Block *rhs = new Block(NumberLiteral); rhs->parameters.push_back(10);
    gt->children.push_back(lhs);
    gt->children.push_back(rhs);
    Block *ifelse = new Block(IfElse);
    ifelse->parameters.push_back(1);
    ifelse->children.push_back(gt);
    Block *mvThen = new Block(Move);
    Block *n10 = new Block(NumberLiteral); n10->parameters.push_back(10);
    mvThen->children.push_back(n10);
    Block *mvElse = new Block(Move);
    Block *n20 = new Block(NumberLiteral); n20->parameters.push_back(20);
    mvElse->children.push_back(n20);
    ifelse->children.push_back(mvThen);
    ifelse->children.push_back(mvElse);
    Block *waitUntil = new Block(WaitUntil);
    Block *gt2 = new Block(Greater);
    Block *lhs2 = new Block(VariableValue); lhs2->text = "score";
    Block *rhs2 = new Block(NumberLiteral); rhs2->parameters.push_back(10);
    gt2->children.push_back(lhs2);
    gt2->children.push_back(rhs2);
    waitUntil->children.push_back(gt2);
    Block *chg = new Block(ChangeVariable);
    chg->text = "score";
    chg->parameters.push_back(10);
    Block *mv5 = new Block(Move);
    Block *n5 = new Block(NumberLiteral); n5->parameters.push_back(5);
    mv5->children.push_back(n5);
    s.blocks.push_back(setv);
    s.blocks.push_back(ifelse);
    s.blocks.push_back(chg);
    s.blocks.push_back(waitUntil);
    s.blocks.push_back(mv5);
    project.scripts.push_back(s);
    MultiRunner mr;
    buildMultiForEvent(project, EVT_GreenFlagClicked, context, mr);
    int safety = 0;
    while (safety++ < 200 && stepMulti(context, project, mr)) {}
    cout << "Final X=" << context.sprite.x
         << " score=" << context.variables["score"] << endl;
    return 0;
}