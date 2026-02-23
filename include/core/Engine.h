#ifndef ENGINE_H
#define ENGINE_H
#include "Block.h"
#include "Context.h"
void executeBlock(Block *block, Context &context);
void pauseExecution(Context &context);
void resumeExecution(Context &context);
#endif