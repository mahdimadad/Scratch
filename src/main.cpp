#include "core/Block.h"
#include "core/Context.h"
#include "core/Engine.h"
#include <iostream>
using namespace std;
int main() {
    Context context;
    Engine engine;
    Block *move1 = new Block(BlockType::Move);
    move1->parameters.push_back(10);
    Block *turn1 = new Block(BlockType::Turn);
    turn1->parameters.push_back(90);
    Block *move2 = new Block(BlockType::Move);
    move2->parameters.push_back(5);
    cout << "Executing script..." << endl;
    engine.executeBlock(move1, context);
    engine.executeBlock(turn1, context);
    engine.executeBlock(move2, context);
    cout << "Final State:" << endl;
    cout << "X = " << context.sprite.x << endl;
    cout << "Direction = " << context.sprite.direction << endl;
    return 0;
}