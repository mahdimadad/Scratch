#ifndef MULTIRUNNER_H
#define MULTIRUNNER_H
#include <vector>
#include "Runner.h"
struct MultiRunner {
    std::vector<Runner> runners;
    int currentIndex = 0;
    bool active = false;
    int nextRunnerId = 1;
};
void buildMultiForEvent(Project &project, EventType eventType, Context &context, MultiRunner &mr);
bool stepMulti(Context &context, Project &project, MultiRunner &mr);
bool isMultiDone(const MultiRunner &mr);
void stopAll(MultiRunner &mr, Context &context);
#endif