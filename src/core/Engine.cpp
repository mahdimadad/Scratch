#include "core/Engine.h"
#include "core/Logger.h"
#include <iostream>
using namespace std;
static void executeMove(Block *block, Context &context);
static void executeTurn(Block *block, Context &context);
static void executeRepeat(Block *block, Context &context);
static void executeSetVariable(Block *block, Context &context);
static void executeChangeVariable(Block *block, Context &context);
void executeBlock(Block *block, Context &context) {
    switch (block->type) {
        case Move: executeMove(block, context);
            break;
        case Turn: executeTurn(block, context);
            break;
        case Repeat: executeRepeat(block, context);
            break;
        case SetVariable: executeSetVariable(block, context);
            break;
        case ChangeVariable: executeChangeVariable(block, context);
            break;
        default: cout << "Block type not implemented yet." << endl;
            break;
    }
}
static void executeMove(Block *block, Context &context) {
    if (block->parameters.empty()) return;
    int steps = block->parameters[0];
    int oldX = context.sprite.x;
    context.sprite.x += steps;
    logMessage(context.currentLine, "MOVE", "Changed X from " + to_string(oldX) + " to " + to_string(context.sprite.x),
               INFO);
}
static void executeTurn(Block *block, Context &context) {
    if (block->parameters.empty()) return;
    int angle = block->parameters[0];
    int oldDir = context.sprite.direction;
    context.sprite.direction += angle;
    logMessage(context.currentLine, "TURN",
               "Changed direction from " + to_string(oldDir) + " to " + to_string(context.sprite.direction), INFO);
}
static void executeRepeat(Block *block, Context &context) {
    if (block->parameters.empty()) return;
    int times = block->parameters[0];
    for (int i = 0; i < times; i++) { for (Block *child: block->children) { executeBlock(child, context); } }
}
static void executeSetVariable(Block *block, Context &context) {
    if (block->text.empty()) return;
    if (block->parameters.empty()) return;
    string name = block->text;
    int value = block->parameters[0];
    int oldValue = context.variables[name];
    context.variables[name] = value;
    logMessage(context.currentLine, "SET_VAR",
               "Changed " + name + " from " + to_string(oldValue) + " to " + to_string(value), INFO);
}
static void executeChangeVariable(Block *block, Context &context) {
    if (block->text.empty()) return;
    if (block->parameters.empty()) return;
    string name = block->text;
    int delta = block->parameters[0];
    int oldValue = context.variables[name];
    context.variables[name] += delta;
    logMessage(context.currentLine, "CHANGE_VAR",
               "Changed " + name + " from " + to_string(oldValue) + " to " + to_string(context.variables[name]), INFO);
}