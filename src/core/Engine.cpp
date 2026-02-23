#include "core/Engine.h"
#include "core/Logger.h"
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
        int oldX = context.sprite.x;
        context.sprite.x += steps;
        Logger::log(context.currentLine, "MOVE",
                    "Changed X from " + to_string(oldX) + " to " + to_string(context.sprite.x), LogLevel::INFO);
    }
}
void Engine::executeTurn(Block *block, Context &context) {
    if (!block->parameters.empty()) {
        int angle = block->parameters[0];
        int oldDir = context.sprite.direction;
        context.sprite.direction += angle;
        Logger::log(context.currentLine, "TURN",
                    "Changed direction from " + to_string(oldDir) + " to " + to_string(context.sprite.direction),
                    LogLevel::INFO);
    }
}
void Engine::executeRepeat(Block *block, Context &context) {
    if (block->parameters.empty())return;
    int times = block->parameters[0];
    for (int i = 0; i < times; i++) { for (Block *child: block->children) { executeBlock(child, context); } }
}