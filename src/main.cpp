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
    setv->parameters.push_back(12);
    Block *gt = new Block(Greater);
    Block *lhs1 = new Block(VariableValue);
    lhs1->text = "score";
    Block *rhs1 = new Block(NumberLiteral);
    rhs1->parameters.push_back(10);
    gt->children.push_back(lhs1);
    gt->children.push_back(rhs1);
    Block *eq = new Block(Equal);
    Block *lhs2 = new Block(VariableValue);
    lhs2->text = "score";
    Block *rhs2 = new Block(NumberLiteral);
    rhs2->parameters.push_back(20);
    eq->children.push_back(lhs2);
    eq->children.push_back(rhs2);
    Block *notEq = new Block(Not);
    notEq->children.push_back(eq);
    Block *andExpr = new Block(And);
    andExpr->children.push_back(gt);
    andExpr->children.push_back(notEq);
    Block *iff = new Block(If);
    iff->children.push_back(andExpr);
    Block *mv = new Block(Move);
    Block *num10 = new Block(NumberLiteral);
    num10->parameters.push_back(10);
    mv->children.push_back(num10);
    iff->children.push_back(mv);
    s.blocks.push_back(setv);
    s.blocks.push_back(iff);
    project.scripts.push_back(s);
    MultiRunner mr;
    buildMultiForEvent(project, EVT_GreenFlagClicked, context, mr);
    int safety = 0;
    while (safety++ < 200 && stepMulti(context, project, mr)) {}
    cout << "Final X=" << context.sprite.x
         << " score=" << context.variables["score"] << endl;
    return 0;
}