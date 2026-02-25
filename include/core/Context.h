#ifndef CONTEXT_H
#define CONTEXT_H
#include "Block.h"
#include <map>
#include <string>
#include <vector>
using namespace std;
struct PendingBroadcast {
    std::string name;
    int parentRunnerId;
    bool wait;
};
struct SpriteState {
    int x = 0;
    int y = 0;
    int direction = 0;
    bool visible = true;
    std::string bubbleText = "";
    bool bubbleIsThink = false;
    unsigned long long bubbleUntilMs = 0;
    int sizePercent = 100;
    int costumeIndex = 0;
    int layer = 0;
    int colorEffect = 0;
    std::vector<std::string> costumes;
};
struct Block;
struct SavedVar {
    std::string name;
    int oldValue;
    bool existed;
};

struct RestoreFrame {
    std::vector<SavedVar> saved;
};
struct Context {
    SpriteState sprite;
    std::map<std::string, Block *> functionTable;
    std::vector<RestoreFrame> restoreStack;
    std::vector<PendingBroadcast> pendingBroadcasts;
    map<string, int> variables;
    bool isRunning = true;
    int currentLine = 0;
    int instructionCounter = 0;
    const int MAX_INSTRUCTIONS_PER_CYCLE = 1000;
    bool stepMode = false;
    bool stepRequested = false;
    bool paused = false;
    int backdropIndex = 0;
    std::vector<std::string> backdrops;
};
#endif