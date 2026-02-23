#include "core/Block.h"
#include "core/Context.h"
#include "core/Engine.h"
#include <iostream>
using namespace std;
int main() {
    Context context;
    Engine engine;
    cout << "Executing Repeat test..." << endl;
    Block *move = new Block(BlockType::Move);
    move->parameters.push_back(10);
    Block *repeat = new Block(BlockType::Repeat);
    repeat->parameters.push_back(3);
    repeat->children.push_back(move);
    engine.executeBlock(repeat, context);
    cout << "Final X = " << context.sprite.x << endl;
    return 0;
}