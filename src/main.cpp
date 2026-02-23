#include "core/Block.h"
#include "core/Context.h"
#include "core/Engine.h"
#include <iostream>
using namespace std;
int main() {
    Context context;
    context.sprite.direction = 90;
    Block *move = new Block(Move);
    move->parameters.push_back(1000);
    executeBlock(move, context);
    cout << "After big move:" << endl;
    cout << "X = " << context.sprite.x << endl;
    cout << "Y = " << context.sprite.y << endl;
    return 0;
}