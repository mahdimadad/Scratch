#include "core/Engine.h"
#include <iostream>
using namespace std;
void Engine::executeBlock(Block *block, Context &context) {
    switch (block->type) {
        case BlockType::Move: executeMove(block, context);
            break;
        case BlockType::Turn: executeTurn(block, context);
            break;
        case BlockType::Repeat: executeRepeat(block, context);
            break;
        default: cout << "Block type not implemented yet." << endl;
            break;
    }
}
void Engine::executeMove(Block *block, Context &context) {
    if (!block->parameters.empty()) {
        int steps = block->parameters[0];
        context.sprite.x += steps;
        cout << "Move executed. New X: " << context.sprite.x << endl;
    }
}
void Engine::executeTurn(Block *block, Context &context) {
    if (!block->parameters.empty()) {
        int angle = block->parameters[0];
        context.sprite.direction += angle;
        cout << "Turn executed. New direction: " << context.sprite.direction << endl;
    }
}
void Engine::executeRepeat(Block *block, Context &context) {
    if (block->parameters.empty())return;
    int times = block->parameters[0];
    for (int i = 0; i < times; i++) { for (Block *child: block->children) { executeBlock(child, context); } }
}