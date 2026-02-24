#ifndef CONTEXT_H
#define CONTEXT_H
#include <map>
#include <string>
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
};
struct Context {
    SpriteState sprite;
    std::map<std::string, Block *> functionTable;
    std::vector<PendingBroadcast> pendingBroadcasts;
    map<string, int> variables;
    bool isRunning = true;
    int currentLine = 0;
};
#endif