#ifndef EVAL_H
#define EVAL_H
#include "Block.h"
#include "Context.h"
int evalValue(Block* expr, Context& context);
bool evalBool(Block* expr, Context& context);
#endif