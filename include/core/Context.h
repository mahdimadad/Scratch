#ifdef CONTEXT_H
#define CONTEXT_H
#include <map>
#include <string>
using namespace std;
struct SpriteState {
    int x = 0;
    int y = 0;
    int direction = 0;
    bool visible = true;
};
struct Context {
    SpriteState sprite;
    map<string, int> variables;
    bool isRunning = true;
    int currentLine = 0;
};
#endif