#ifndef BLOCK_H
#define BLOCK_H
#include <vector>
#include <string>
using namespace std;

enum BlockType {
    Move, Turn, IfElse, NumberLiteral, WaitUntil, Repeat, Say, SayForSeconds, Think, ThinkForSeconds, If, RestoreVars,
    CustomFunctionCall, SetVariable, ChangeVariable, Wait, Forever, CallFunction, DefineFunction, RepeatUntil, Number,
    Variable, Add, Sub, Mul, Div, Equal, Greater, Less, And, Or, Not, VariableValue, MessageReceived, Broadcast,
    BroadcastAndWait, GreenFlagClicked, Show, Hide, SetSize, ChangeSize, NextCostume, SetCostume, NextBackdrop,
    SetBackdrop, ClearEffects, SetColorEffect, ChangeColorEffect, GoToFrontLayer, GoToBackLayer, GoForwardLayers,
    GoBackwardLayers, ReporterSize, ReporterCostumeNumber, ReporterBackdropNumber
};
struct Block {
    std::vector<std::string> paramNames;
    BlockType type;
    vector<Block *> children;
    vector<int> parameters;
    std::string text;
    Block(BlockType t) { type = t; }
};
#endif