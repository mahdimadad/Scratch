#ifndef ENGINE_H
#define ENGINE_H
#include "Block.h"
#include "Context.h"
class Engine {
public:
    void executeBlock(Block *block, Context &context);
private:
    void executeMove(Block *block, Context &context);
    void executeTurn(Block *block, Context &context);
    void executeRepeat(Block *block, Context &context);
};
#endif