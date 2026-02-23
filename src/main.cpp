#include "core/Block.h"
#include "core/Context.h"
#include "core/Engine.h"
#include <iostream>
using namespace std;
int main() {
    Context context;
    cout << "Executing Repeat test..." << endl;
    Block *move = new Block(Move);
    move->parameters.push_back(10);
    Block *repeat = new Block(Repeat);
    repeat->parameters.push_back(3);
    repeat->children.push_back(move);
    executeBlock(repeat, context);
    cout << "Final X = " << context.sprite.x << endl;
    cout << "\nExecuting Variable test..." << endl;
    Block *setv = new Block(SetVariable);
    setv->text = "score";
    setv->parameters.push_back(10);
    Block *chv = new Block(ChangeVariable);
    chv->text = "score";
    chv->parameters.push_back(5);
    executeBlock(setv, context);
    executeBlock(chv, context);
    cout << "score = " << context.variables["score"] << endl;
    return 0;
}