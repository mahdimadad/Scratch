#ifndef EVENTS_H
#define EVENTS_H
#include <vector>
#include <string>
#include "Block.h"
#include "Context.h"
enum EventType { EVT_GreenFlagClicked, EVT_MessageReceived };
struct Script {
    EventType eventType;
    std::string messageName;
    std::vector<Block *> blocks;
};
struct Project {
    std::vector<Script> scripts;
};
void runEvent(Project &project, EventType eventType, Context &context);
#endif