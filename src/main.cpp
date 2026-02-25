#include "core/Context.h"
#include "core/Block.h"
#include "core/Events.h"
#include "core/MultiRunner.h"
#include <iostream>
using namespace std;
int main() {
    Context context;
    context.isRunning = true;
    context.sprite.sizePercent = 150;
    context.sprite.costumes = {"c1", "c2", "c3"};
    context.sprite.costumeIndex = 2;
    context.backdrops = {"b1", "b2"};
    context.backdropIndex = 1;
    Project project;
    Script s;
    s.eventType = EVT_GreenFlagClicked;
    Block *cond = new Block(Equal);
    Block *lhs = new Block(ReporterSize);
    Block *rhs = new Block(NumberLiteral);
    rhs->parameters.push_back(150);
    cond->children.push_back(lhs);
    cond->children.push_back(rhs);
    Block *iff = new Block(If);
    iff->children.push_back(cond);
    Block *mv = new Block(Move);
    Block *n10 = new Block(NumberLiteral);
    n10->parameters.push_back(10);
    mv->children.push_back(n10);
    iff->children.push_back(mv);
    s.blocks.push_back(iff);
    project.scripts.push_back(s);
    MultiRunner mr;
    buildMultiForEvent(project, EVT_GreenFlagClicked, context, mr);
    int safety = 0;
    while (safety++ < 50 && stepMulti(context, project, mr)) {}
    cout << "Final X=" << context.sprite.x << "\n";
    cout << "size=" << context.sprite.sizePercent
         << " costume#=" << (context.sprite.costumeIndex + 1)
         << " backdrop#=" << (context.backdropIndex + 1) << "\n";
    return 0;
}