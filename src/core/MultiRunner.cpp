#include "core/MultiRunner.h"
void buildMultiForEvent(Project &project, EventType eventType, Context &context, MultiRunner &mr) {
    mr.runners.clear();
    mr.currentIndex = 0;
    mr.active = true;
    context.isRunning = true;
    for (Script &s: project.scripts) {
        if (s.eventType != eventType)continue;
        Runner r;
        buildQueueForScript(s, context, r);
        mr.runners.push_back(r);
    }
}
bool stepMulti(Context &context, MultiRunner &mr) {
    if (!mr.active) return false;
    if (!context.isRunning) return false;
    if (mr.runners.empty()) return false;
    int n = (int) mr.runners.size();
    for (int i = 0; i < n; i++) {
        int idx = (mr.currentIndex + i) % n;
        Runner &r = mr.runners[idx];
        if (!isRunnerDone(r)) {
            mr.currentIndex = (idx + 1) % n;
            return stepRunner(context, r);
        }
    }
    return false;
}
bool isMultiDone(const MultiRunner &mr) {
    if (!mr.active) return true;
    for (const Runner &r: mr.runners) { if (!isRunnerDone(r))return false; }
    return true;
}
void stopAll(MultiRunner &mr, Context &context) {
    mr.active = false;
    mr.currentIndex = 0;
    mr.runners.clear();
    context.isRunning = false;
}