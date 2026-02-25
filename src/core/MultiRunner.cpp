#include "core/MultiRunner.h"
#include "core/Runner.h"
#include "core/Logger.h"
void buildMultiForEvent(Project &project, EventType eventType, Context &context, MultiRunner &mr) {
    registerFunctions(project, context);
    mr.runners.clear();
    mr.currentIndex = 0;
    mr.nextRunnerId = 0;
    mr.active = true;
    for (Script &s: project.scripts) {
        if (s.eventType == eventType) {
            Runner r;
            buildQueueForScript(s, context, r);
            r.id = mr.nextRunnerId++;
            r.parentId = -1;
            r.waitChildren = 0;
            r.blocked = false;
            mr.runners.push_back(r);
        }
    }
}
static void cleanupDoneRunners(MultiRunner &mr) {
    std::vector<Runner> alive;
    alive.reserve(mr.runners.size());

    for (auto &r : mr.runners) {
        if (!isRunnerDone(r)) {
            alive.push_back(r);
        }
    }

    mr.runners.swap(alive);

    if (mr.currentIndex >= (int)mr.runners.size()) {
        mr.currentIndex = 0;
    }

    if (mr.runners.empty()) {
        mr.active = false;
        mr.currentIndex = 0;
    }
}
bool stepMulti(Context &context, Project &project, MultiRunner &mr) {
    if (context.paused) return false;
    static int cycleCounter = 0;
    cycleCounter++;
    Logger::setCycle(cycleCounter);
    if (!mr.active) return false;
    if (!context.isRunning) return false;
    if (!context.pendingBroadcasts.empty()) {
        for (const PendingBroadcast &pb: context.pendingBroadcasts) {
            int spawned = 0;
            for (Script &s: project.scripts) {
                if (s.eventType == EVT_MessageReceived && s.messageName == pb.name) {
                    Runner r;
                    buildQueueForScript(s, context, r);
                    r.id = mr.nextRunnerId++;
                    r.parentId = pb.wait ? pb.parentRunnerId : -1;
                    r.waitChildren = 0;
                    r.blocked = false;
                    mr.runners.push_back(r);
                    spawned++;
                }
            }
            if (pb.wait && pb.parentRunnerId != -1) {
                for (Runner &parent: mr.runners) {
                    if (parent.id == pb.parentRunnerId) {
                        parent.waitChildren += spawned;
                        if (parent.waitChildren == 0)parent.blocked = false;
                        break;
                    }
                }
            }
        }
        context.pendingBroadcasts.clear();
    }
    if (mr.runners.empty()) return false;
    for (Runner &child: mr.runners) {
        if (isRunnerDone(child) && child.parentId != -1) {
            for (Runner &parent: mr.runners) {
                if (parent.id == child.parentId) {
                    if (parent.waitChildren > 0)parent.waitChildren--;
                    if (parent.waitChildren == 0)parent.blocked = false;
                    break;
                }
            }
            child.parentId = -1;
        }
    }
    int n = (int)mr.runners.size();

    if (context.stepMode && !context.stepRequested)
        return false;

    for (int i = 0; i < n; i++) {
        int idx = (mr.currentIndex + i) % n;
        Runner &r = mr.runners[idx];

        if (!isRunnerDone(r) && !r.blocked) {
            mr.currentIndex = (idx + 1) % n;

            bool did = stepRunner(context, r);

            if (context.stepMode && did)
                context.stepRequested = false;

            return did;
        }
    }
    cleanupDoneRunners(mr);
    if (!mr.active) return false;
    return false;
}
bool isMultiDone(const MultiRunner &mr) {
    if (!mr.active) return true;
    for (const Runner &r: mr.runners) { if (!isRunnerDone(r))return false; }
    return true;
}
void stopAll(MultiRunner &mr, Context &context) {

    mr.runners.clear();
    mr.active = false;
    mr.currentIndex = 0;

    context.pendingBroadcasts.clear();
    context.isRunning = false;

    Logger::log(
        LOG_INFO,
        "ENGINE",
        "Stop All executed"
    );
}