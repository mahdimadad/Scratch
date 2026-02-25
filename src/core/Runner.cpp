#include "core/Runner.h"
#include "core/Engine.h"
#include "core/Context.h"
#include "core/Logger.h"
#include "core/Eval.h"
#include <chrono>
#include <iostream>
static bool checkIfCondition(Block *block, Context &context) {
    if (block->text.empty()) return false;
    if (block->parameters.size() < 2) return false;
    std::string name = block->text;
    int op = block->parameters[0];
    int rhs = block->parameters[1];
    int lhs = context.variables[name];
    if (op == 0) return lhs == rhs;
    if (op == 1) return lhs > rhs;
    if (op == 2) return lhs < rhs;
    return false;
}
static void appendBlockToQueue(Block *block, Context &context, std::vector<Block *> &q) {
    if (!block) return;
    if (block->type == Repeat) {
        if (block->parameters.empty()) return;
        int times = block->parameters[0];
        for (int i = 0; i < times; i++) {
            for (Block *child: block->children) { appendBlockToQueue(child, context, q); }
        }
        return;
    }
    if (block->type == If) {
        q.push_back(block);
        return;
    }
    if (block->type == Forever) {
        q.push_back(block);
        return;
    }
    if (block->type == DefineFunction) {
        context.functionTable[block->text] = block;
        return;
    }
    if (block->type == RepeatUntil) {
        q.push_back(block);
        return;
    }
    if (block->type == RepeatUntil) {
        q.push_back(block);
        return;
    }
    q.push_back(block);
}
void buildQueueForEvent(Project &project, EventType eventType, Context &context, Runner &runner) {
    runner.queue.clear();
    runner.index = 0;
    runner.active = true;
    context.currentLine = 0;
    for (Script &s: project.scripts) {
        if (s.eventType != eventType)continue;
        for (Block *b: s.blocks) { appendBlockToQueue(b, context, runner.queue); }
    }
}
static unsigned long long nowMs() {
    using namespace std::chrono;
    return (unsigned long long) duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
}
bool stepRunner(Context &context, Runner &runner) {
    if (context.paused) return false;
    if (!runner.active) return false;
    if (!context.isRunning) return false;
    if (runner.waitUntilMs > 0) {
        if (nowMs() < runner.waitUntilMs) { return false; }
        runner.waitUntilMs = 0;
    }
    while (runner.index < (int) runner.queue.size()) {
        Block *b = runner.queue[runner.index];
        runner.index++;
        if (b->type == Wait) {
            if (!b->parameters.empty()) {
                int seconds = b->parameters[0];
                if (seconds < 0) seconds = 0;
                runner.waitUntilMs = nowMs() + (unsigned long long) (seconds * 1000);
            }
            return false;
        }
        if (b->type == If) {
            if (b->children.size() >= 1) {
                Block *cond = b->children[0];
                if (!evalBool(cond, context)) { continue; }
                for (int i = (int) b->children.size() - 1; i >= 1; i--) {
                    runner.queue.insert(runner.queue.begin() + runner.index, b->children[i]);
                }
                continue;
            }
            if (b->text.empty() || b->parameters.size() < 2) continue;
            std::string name = b->text;
            int op = b->parameters[0];
            int rhs = b->parameters[1];
            int lhs = context.variables[name];
            bool ok = false;
            if (op == 0) ok = (lhs == rhs);
            if (op == 1) ok = (lhs > rhs);
            if (op == 2) ok = (lhs < rhs);
            if (!ok) continue;
            for (int i = (int) b->children.size() - 1; i >= 0; i--) {
                runner.queue.insert(runner.queue.begin() + runner.index, b->children[i]);
            }
            continue;
        }
        if (b->type == Forever) {
            for (int i = (int) b->children.size() - 1; i >= 0; i--) {
                runner.queue.insert(runner.queue.begin() + runner.index, b->children[i]);
            }
            runner.queue.insert(runner.queue.begin() + runner.index + (int) b->children.size(), b);
            continue;
        }
                if (b->type == CallFunction) {
            std::string fname = b->text;
            if (fname.empty()) {
                continue;
            }
            if (!context.functionTable.count(fname)) {
                Logger::log(LOG_WARNING, "FUNC", "Function not found: " + fname);
                continue;
            }
            Block *def = context.functionTable[fname];
            int argCount = (int)b->children.size();
            int paramCount = (int)def->paramNames.size();
            if (argCount < paramCount) {
                Logger::log(LOG_WARNING, "FUNC", "Not enough args for: " + fname);
                continue;
            }

            RestoreFrame frame;
            for (int i = 0; i < paramCount; i++) {
                const std::string &pname = def->paramNames[i];
                SavedVar sv;
                sv.name = pname;
                auto it = context.variables.find(pname);
                if (it != context.variables.end()) {
                    sv.existed = true;
                    sv.oldValue = it->second;
                } else {
                    sv.existed = false;
                    sv.oldValue = 0;
                }
                frame.saved.push_back(sv);

                int value = evalInt(b->children[i], context);
                context.variables[pname] = value;
            }

            int frameId = (int)context.restoreStack.size();
            context.restoreStack.push_back(frame);

            Block *restore = new Block(RestoreVars);
            restore->parameters.push_back(frameId);

            std::vector<Block*> expanded;
            for (Block *ch : def->children) {
                appendBlockToQueue(ch, context, expanded);
            }

            runner.queue.insert(runner.queue.begin() + runner.index, restore);

            for (int i = (int)expanded.size() - 1; i >= 0; i--) {
                runner.queue.insert(runner.queue.begin() + runner.index, expanded[i]);
            }

            Logger::log(LOG_INFO, "FUNC", "Call " + fname);
            continue;
        }
        if (b->type == BroadcastAndWait) {
            PendingBroadcast p;
            p.name = b->text;
            p.parentRunnerId = runner.id;
            p.wait = true;
            context.pendingBroadcasts.push_back(p);
            runner.blocked = true;
            return true;
        }
        executeBlock(b, context);
        return true;
    }
    runner.active = false;
    return false;
}
bool isRunnerDone(const Runner &r) { return r.index >= (int) r.queue.size(); }
static void appendBlockToQueue(Block *block, std::vector<Block *> &q) {
    if (!block) return;
    if (block->type == Repeat) {
        if (block->parameters.empty()) return;
        int times = block->parameters[0];
        for (int i = 0; i < times; i++) { for (Block *child: block->children) { appendBlockToQueue(child, q); } }
        return;
    }
    q.push_back(block);
}
void buildQueueForScript(Script &script, Context &context, Runner &runner) {
    runner.queue.clear();
    runner.index = 0;
    runner.active = true;
    runner.waitUntilMs = 0;
    for (Block *b: script.blocks) { appendBlockToQueue(b, context, runner.queue); }
}
void enableStepMode(Context &context) {
    context.stepMode = true;
    context.stepRequested = false;
}

void requestStep(Context &context) {
    context.stepRequested = true;
}
static bool containsEventBlock(Block *b) {
    if (!b) return false;
    for (Block *ch : b->children) {
        if (containsEventBlock(ch)) return true;
    }
    return false;
}
void registerFunctions(Project &project, Context &context) {
    context.functionTable.clear();
    for (Script &s : project.scripts) {
        for (Block *b : s.blocks) {
            if (!b) continue;
            if (b->type != DefineFunction) continue;

            if (b->text.empty()) {
                Logger::log(LOG_WARNING, "FUNC", "DefineFunction with empty name ignored");
                continue;
            }
            bool bad = false;
            for (Block *body : b->children) {
                if (containsEventBlock(body)) {
                    bad = true;
                    break;
                }
            }
            if (bad) {
                Logger::log(LOG_ERROR, "FUNC", "Event block inside function '" + b->text + "'. Not registered.");
                continue;
            }
            if (context.functionTable.count(b->text)) {
                Logger::log(LOG_WARNING, "FUNC", "Duplicate function name '" + b->text + "'. Overriding.");
            }
            context.functionTable[b->text] = b;
        }
    }
}