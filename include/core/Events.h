#ifndef EVENTS_H
#define EVENTS_H
#include <vector>
#include "Block.h"
#include "Context.h"
enum EventType { GreenFlagClicked };
struct Script {
    EventType eventType;
    std::vector<Block *> blocks;
};
struct Project {
    std::vector<Script> scripts;
};
void runEvent(Project &project, EventType eventType, Context &context);
#endif