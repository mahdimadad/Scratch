#include "core/Engine.h"
#include "core/Logger.h"
#include "core/Utils.h"
#include "core/Block.h"
#include "core/Eval.h"
#include "core/Context.h"
#include <cmath>
using namespace std;

static void executeMove(Block *block, Context &context);

static void executeTurn(Block *block, Context &context);

static void executeRepeat(Block *block, Context &context);

static void executeSetVariable(Block *block, Context &context);

static void executeChangeVariable(Block *block, Context &context);

static void executeIf(Block *block, Context &context);

static bool checkCondition(Block *block, Context &context);

void executeBlock(Block *block, Context &context) {
    if (!context.isRunning) return;
    context.instructionCounter++;

    if (context.instructionCounter > context.MAX_INSTRUCTIONS_PER_CYCLE) {
        Logger::log(
            LOG_ERROR,
            "WATCHDOG",
            "Infinite loop detected. Execution stopped."
        );

        context.isRunning = false;
        return;
    }
    if (context.sprite.bubbleUntilMs > 0 && nowMs() >= context.sprite.bubbleUntilMs) {
        context.sprite.bubbleText = "";
        context.sprite.bubbleUntilMs = 0;
    }
    switch (block->type) {
        case Move:
            executeMove(block, context);
            break;
        case Turn:
            executeTurn(block, context);
            break;
        case Repeat:
            executeRepeat(block, context);
            break;
        case SetVariable:
            executeSetVariable(block, context);
            break;
        case ChangeVariable:
            executeChangeVariable(block, context);
            break;
        case If:
            executeIf(block, context);
            break;
        case Show: {
    context.sprite.visible = true;
    Logger::log(LOG_INFO, "LOOKS", "Show");
    break;
}
case Hide: {
    context.sprite.visible = false;
    Logger::log(LOG_INFO, "LOOKS", "Hide");
    break;
}

case SetSize: {
    int v = 100;
    if (!block->parameters.empty()) v = block->parameters[0];
    if (v < 0) v = 0;
    if (v > 500) v = 500;
    context.sprite.sizePercent = v;
    Logger::log(LOG_INFO, "LOOKS", "Set size to " + std::to_string(v));
    break;
}
case ChangeSize: {
    int d = 0;
    if (!block->parameters.empty()) d = block->parameters[0];
    int v = context.sprite.sizePercent + d;
    if (v < 0) v = 0;
    if (v > 500) v = 500;
    context.sprite.sizePercent = v;
    Logger::log(LOG_INFO, "LOOKS", "Change size by " + std::to_string(d) + " -> " + std::to_string(v));
    break;
}

case NextCostume: {
    if (!context.sprite.costumes.empty()) {
        context.sprite.costumeIndex = (context.sprite.costumeIndex + 1) % (int)context.sprite.costumes.size();
        Logger::log(LOG_INFO, "LOOKS", "Next costume -> " + std::to_string(context.sprite.costumeIndex));
    } else {
        Logger::log(LOG_WARNING, "LOOKS", "Next costume ignored (no costumes)");
    }
    break;
}
case SetCostume: {
    if (block->text.empty()) {
        Logger::log(LOG_WARNING, "LOOKS", "Set costume ignored (empty name)");
        break;
    }
    int idx = -1;
    for (int i = 0; i < (int)context.sprite.costumes.size(); i++) {
        if (context.sprite.costumes[i] == block->text) { idx = i; break; }
    }
    if (idx == -1) {
        Logger::log(LOG_WARNING, "LOOKS", "Costume not found: " + block->text);
    } else {
        context.sprite.costumeIndex = idx;
        Logger::log(LOG_INFO, "LOOKS", "Set costume -> " + block->text);
    }
    break;
}

case NextBackdrop: {
    if (!context.backdrops.empty()) {
        context.backdropIndex = (context.backdropIndex + 1) % (int)context.backdrops.size();
        Logger::log(LOG_INFO, "LOOKS", "Next backdrop -> " + std::to_string(context.backdropIndex));
    } else {
        Logger::log(LOG_WARNING, "LOOKS", "Next backdrop ignored (no backdrops)");
    }
    break;
}
case SetBackdrop: {
    if (block->text.empty()) {
        Logger::log(LOG_WARNING, "LOOKS", "Set backdrop ignored (empty name)");
        break;
    }
    int idx = -1;
    for (int i = 0; i < (int)context.backdrops.size(); i++) {
        if (context.backdrops[i] == block->text) { idx = i; break; }
    }
    if (idx == -1) {
        Logger::log(LOG_WARNING, "LOOKS", "Backdrop not found: " + block->text);
    } else {
        context.backdropIndex = idx;
        Logger::log(LOG_INFO, "LOOKS", "Set backdrop -> " + block->text);
    }
    break;
}

case ClearEffects: {
    context.sprite.colorEffect = 0;
    Logger::log(LOG_INFO, "LOOKS", "Clear effects");
    break;
}
case SetColorEffect: {
    int v = 0;
    if (!block->parameters.empty()) v = block->parameters[0];
    context.sprite.colorEffect = v;
    Logger::log(LOG_INFO, "LOOKS", "Set color effect -> " + std::to_string(v));
    break;
}
case ChangeColorEffect: {
    int d = 0;
    if (!block->parameters.empty()) d = block->parameters[0];
    context.sprite.colorEffect += d;
    Logger::log(LOG_INFO, "LOOKS", "Change color effect by " + std::to_string(d) +
                                   " -> " + std::to_string(context.sprite.colorEffect));
    break;
}

case GoToFrontLayer: {
    context.sprite.layer = 1000000;
    Logger::log(LOG_INFO, "LOOKS", "Go to front layer");
    break;
}
case GoToBackLayer: {
    context.sprite.layer = -1000000;
    Logger::log(LOG_INFO, "LOOKS", "Go to back layer");
    break;
}
case GoForwardLayers: {
    int n = 0;
    if (!block->parameters.empty()) n = block->parameters[0];
    context.sprite.layer += n;
    Logger::log(LOG_INFO, "LOOKS", "Go forward layers " + std::to_string(n) +
                                   " -> " + std::to_string(context.sprite.layer));
    break;
}
case GoBackwardLayers: {
    int n = 0;
    if (!block->parameters.empty()) n = block->parameters[0];
    context.sprite.layer -= n;
    Logger::log(LOG_INFO, "LOOKS", "Go backward layers " + std::to_string(n) +
                                   " -> " + std::to_string(context.sprite.layer));
    break;
}
        case Say: {
            context.sprite.bubbleText = block->text;
            context.sprite.bubbleIsThink = false;
            context.sprite.bubbleUntilMs = 0;

            Logger::log(LOG_INFO, "SAY", "Say: " + context.sprite.bubbleText);
            break;
        }

        case Think: {
            context.sprite.bubbleText = block->text;
            context.sprite.bubbleIsThink = true;
            context.sprite.bubbleUntilMs = 0;

            Logger::log(LOG_INFO, "THINK", "Think: " + context.sprite.bubbleText);
            break;
        }

        case SayForSeconds: {
            int sec = 0;
            if (!block->parameters.empty()) sec = block->parameters[0];
            if (sec < 0) sec = 0;

            context.sprite.bubbleText = block->text;
            context.sprite.bubbleIsThink = false;
            context.sprite.bubbleUntilMs = nowMs() + (unsigned long long)sec * 1000ULL;

            Logger::log(LOG_INFO, "SAY", "Say for " + std::to_string(sec) + "s: " + context.sprite.bubbleText);
            break;
        }

        case ThinkForSeconds: {
            int sec = 0;
            if (!block->parameters.empty()) sec = block->parameters[0];
            if (sec < 0) sec = 0;

            context.sprite.bubbleText = block->text;
            context.sprite.bubbleIsThink = true;
            context.sprite.bubbleUntilMs = nowMs() + (unsigned long long)sec * 1000ULL;

            Logger::log(LOG_INFO, "THINK", "Think for " + std::to_string(sec) + "s: " + context.sprite.bubbleText);
            break;
        }
        case RestoreVars: {
            if (block->parameters.empty()) break;
            int frameId = block->parameters[0];
            if (frameId < 0 || frameId >= (int)context.restoreStack.size()) break;
            RestoreFrame &frame = context.restoreStack[frameId];
            for (SavedVar &sv : frame.saved) {
                if (sv.existed) {
                    context.variables[sv.name] = sv.oldValue;
                } else {
                    context.variables.erase(sv.name);
                }
            }
            Logger::log(LOG_INFO, "FUNC", "Restore vars (frame " + std::to_string(frameId) + ")");
            break;
        }
        case Broadcast: {
            PendingBroadcast p;
            p.name = block->text;
            p.parentRunnerId = -1;
            p.wait = false;
            context.pendingBroadcasts.push_back(p);
            Logger::log(
                LOG_INFO,
                "BROADCAST",
                "Broadcast message: " + block->text
            );
            break;
        }
        default:
            Logger::log(
                LOG_WARNING,
                "ENGINE",
                "Block type not implemented"
            );
            break;
    }
}

static void executeMove(Block *block, Context &context) {
    int steps = 0;

    if (!block->children.empty() && block->children[0] != nullptr) {
        steps = evalInt(block->children[0], context);
    } else if (!block->parameters.empty()) {
        steps = block->parameters[0];
    } else {
        return;
    }

    int oldX = context.sprite.x;
    int oldY = context.sprite.y;

    double radians = context.sprite.direction * 3.1415926535 / 180.0;

    context.sprite.x += static_cast<int>(steps * cos(radians));
    context.sprite.y += static_cast<int>(steps * sin(radians));

    int beforeClampX = context.sprite.x;
    int beforeClampY = context.sprite.y;

    context.sprite.x = clampInt(context.sprite.x, -240, 240);
    context.sprite.y = clampInt(context.sprite.y, -180, 180);

    if (context.sprite.x != beforeClampX || context.sprite.y != beforeClampY) {
        Logger::log(
            LOG_WARNING,
            "BOUNDARY",
            "Clamped position from (" +
            to_string(beforeClampX) + "," +
            to_string(beforeClampY) + ") to (" +
            to_string(context.sprite.x) + "," +
            to_string(context.sprite.y) + ")"
        );
    }
    Logger::log(
        LOG_INFO,
        "MOVE",
        "Moved from (" +
        to_string(oldX) + "," +
        to_string(oldY) + ") to (" +
        to_string(context.sprite.x) + "," +
        to_string(context.sprite.y) + ")"
    );
}

static void executeTurn(Block *block, Context &context) {
    if (block->parameters.empty()) return;
    int angle = block->parameters[0];
    int oldDir = context.sprite.direction;
    context.sprite.direction += angle;
    context.sprite.direction %= 360;
    if (context.sprite.direction < 0) context.sprite.direction += 360;
    Logger::log(
        LOG_INFO,
        "TURN",
        "Changed direction from " +
        to_string(oldDir) + " to " +
        to_string(context.sprite.direction)
    );
}

static void executeRepeat(Block *block, Context &context) {
    if (block->parameters.empty()) return;

    int times = block->parameters[0];

    for (int i = 0; i < times; i++) { for (Block *child: block->children) { executeBlock(child, context); } }
}

static void executeSetVariable(Block *block, Context &context) {
    if (block->text.empty()) return;
    if (block->parameters.empty()) return;
    string name = block->text;
    int value = block->parameters[0];
    int oldValue = context.variables[name];
    context.variables[name] = value;
    Logger::log(LOG_INFO, "SET_VAR", "Changed " + name + " from " + to_string(oldValue) + " to " + to_string(value));
}

static void executeChangeVariable(Block *block, Context &context) {
    if (block->text.empty()) return;
    if (block->parameters.empty()) return;
    string name = block->text;
    int delta = block->parameters[0];
    int oldValue = context.variables[name];
    context.variables[name] += delta;
    Logger::log(LOG_INFO, "CHANGE_VAR",
                "Changed " + name + " from " + to_string(oldValue) + " to " + to_string(context.variables[name]));
}

void pauseExecution(Context &context) {
    context.paused = true;
    Logger::log(LOG_INFO, "ENGINE", "Execution paused");
}

void resumeExecution(Context &context) {
    context.paused = false;
    Logger::log(LOG_INFO, "ENGINE", "Execution resumed");
}

static bool checkCondition(Block *block, Context &context) {
    if (block->text.empty()) return false;
    if (block->parameters.size() < 2) return false;
    string name = block->text;
    int op = block->parameters[0];
    int rhs = block->parameters[1];
    int lhs = context.variables[name];
    if (op == 0) return lhs == rhs;
    if (op == 1) return lhs > rhs;
    if (op == 2) return lhs < rhs;
    return false;
}

static void executeIf(Block *block, Context &context) {
    if (!checkCondition(block, context)) return;
    for (Block *child: block->children) {
        executeBlock(child, context);
    }
}