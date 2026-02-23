#ifndef CONTEXT_H
#define CONTEXT_H
#include <map>
#include <string>
#include "core/Block.h"
using namespace std;
struct SpriteState {
    int x = 0;
    int y = 0;
    int direction = 0;
    bool visible = true;
};
struct Context {
    SpriteState sprite;
    std::map<std::string, Block *> functionTable;
    map<string, int> variables;
    bool isRunning = true;
    int currentLine = 0;
};
#endif