#ifndef RUNNER_H
#define RUNNER_H
#include <vector>
#include "Block.h"
#include "Context.h"
#include "Events.h"
struct Runner {
    unsigned long long waitUntilMs = 0;
    std::vector<Block *> queue;
    int index = 0;
    bool active = false;
    int id;
    int parentId;
    int waitChildren;
    bool blocked;
};
void buildQueueForScript(Script &script, Context &context, Runner &runner);
void buildQueueForEvent(Project &project, EventType eventType, Context &context, Runner &runner);
bool stepRunner(Context &context, Runner &runner);
bool isRunnerDone(const Runner &runner);
void enableStepMode(Context &context);
void requestStep(Context &context);
void registerFunctions(Project &project, Context &context);
#endif