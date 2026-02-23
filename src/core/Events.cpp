#include "core/Events.h"
#include "core/Engine.h"
void runEvent(Project &project, EventType eventType, Context &context) {
    for (Script &s: project.scripts) {
        if (s.eventType == eventType) { for (Block *b: s.blocks) { executeBlock(b, context); } }
    }
}