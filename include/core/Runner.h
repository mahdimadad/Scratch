#ifndef RUNNER_H
#define RUNNER_H
#include <vector>
#include "Block.h"
#include "Context.h"
#include "Events.h"
struct Runner {
    std::vector<Block *> queue;
    int index = 0;
    bool active = false;
};
void buildQueueForEvent(Project &project, EventType eventType, Context &context, Runner &runner);
bool stepRunner(Context &context, Runner &runner);
bool isRunnerDon(const Runner &runner);
#endif