#include "core/Runner.h"
#include "core/Engine.h"
#include "core/Context.h"
#include "core/Eval.h"
#include <chrono>
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
            if (context.functionTable.count(fname)) {
                Block *def = context.functionTable[fname];
                for (int i = (int) def->children.size() - 1; i >= 0; i--) {
                    runner.queue.insert(runner.queue.begin() + runner.index, def->children[i]);
                }
            }
            continue;
        }
        if (b->type == RepeatUntil) {
            if (b->children.empty()) { continue; }
            Block *cond = b->children[0];
            if (evalBool(cond, context)) { continue; }
            runner.queue.insert(runner.queue.begin() + runner.index, b);
            for (int i = (int) b->children.size() - 1; i >= 1; i--) {
                runner.queue.insert(runner.queue.begin() + runner.index, b->children[i]);
            }
            continue;
        }
        executeBlock(b, context);
        return true;
    }
    return false;
}
bool isRunnerDone(const Runner &runner) { return !runner.active || runner.index >= (int) runner.queue.size(); }
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