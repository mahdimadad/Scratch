#include "core/Engine.h"
#include "core/Logger.h"
#include <iostream>
#include <cmath>
using namespace std;
static void executeMove(Block *block, Context &context);
static void executeTurn(Block *block, Context &context);
static void executeRepeat(Block *block, Context &context);
static void executeSetVariable(Block *block, Context &context);
static void executeChangeVariable(Block *block, Context &context);
static void executeIf(Block *block, Context &context);
void executeBlock(Block *block, Context &context) {
    if (!context.isRunning)return;
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
        case If: executeIf(block, context);
            break;
        default: cout << "Block type not implemented yet." << endl;
            break;
    }
}
static void executeMove(Block *block, Context &context) {
    if (block->parameters.empty()) return;
    ++context.currentLine;
    int steps = block->parameters[0];
    int oldX = context.sprite.x;
    int oldY = context.sprite.y;
    double radians = context.sprite.direction * 3.1415926535 / 180.0;
    context.sprite.x += (int) (steps * cos(radians));
    context.sprite.y += (int) (steps * sin(radians));
    logMessage(context.currentLine, "MOVE",
               "Moved from (" + to_string(oldX) + "," + to_string(oldY) + ") to (" + to_string(context.sprite.x) + "," +
               to_string(context.sprite.y) + ")", INFO);
}
static void executeTurn(Block *block, Context &context) {
    if (block->parameters.empty()) return;
    context.currentLine++;
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
    context.currentLine++;
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
    context.currentLine++;
    string name = block->text;
    int delta = block->parameters[0];
    int oldValue = context.variables[name];
    context.variables[name] += delta;
    logMessage(context.currentLine, "CHANGE_VAR",
               "Changed " + name + " from " + to_string(oldValue) + " to " + to_string(context.variables[name]), INFO);
}
void pauseExecution(Context &context) { context.isRunning = false; }
void resumeExecution(Context &context) { context.isRunning = true; }
static bool checkCondition(Block *block, Context &context) {
    if (block->text.empty()) return false;
    if (block->parameters.size() < 2) return false;
    string name = block->text;
    int op = block->parameters[0];
    int rhs = block->parameters[1];
    int lhs = context.variables[name];
    if (op == 0) return lhs == rhs;
    if (op == 1) return lhs > rhs;
    if (op == 2) return lhs < rhs;
    return false;
}
static void executeIf(Block *block, Context &context) {
    if (!checkCondition(block, context))return;
    for (Block *child: block->children) { executeBlock(child, context); }
}