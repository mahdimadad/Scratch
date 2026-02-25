#include "core/Eval.h"
#include "core/Logger.h"
static int safeDiv(int a, int b) {
    if (b == 0) return 0;
    return a / b;
}
int evalValue(Block *expr, Context &context) {
    if (!expr) return 0;
    if (expr->type == NumberLiteral) {
        if (expr->parameters.empty()) return 0;
        return expr->parameters[0];
    }
    if (expr->type == VariableValue) {
        if (expr->text.empty()) return 0;
        return context.variables[expr->text];
    }
    if (expr->type == ReporterSize) {
        return context.sprite.sizePercent;
    }
    if (expr->type == ReporterCostumeNumber) {
        return context.sprite.costumes.empty() ? 0 : (context.sprite.costumeIndex + 1);
    }
    if (expr->type == ReporterBackdropNumber) {
        return context.backdrops.empty() ? 0 : (context.backdropIndex + 1);
    }
    if (expr->type == Add) {
        if (expr->children.size() < 2) return 0;
        return evalValue(expr->children[0], context) + evalValue(expr->children[1], context);
    }
    if (expr->type == Sub) {
        if (expr->children.size() < 2) return 0;
        return evalValue(expr->children[0], context) - evalValue(expr->children[1], context);
    }
    if (expr->type == Mul) {
        if (expr->children.size() < 2) return 0;
        return evalValue(expr->children[0], context) * evalValue(expr->children[1], context);
    }
    if (expr->type == Div) {
        if (expr->children.size() < 2) return 0;
        int a = evalValue(expr->children[0], context);
        int b = evalValue(expr->children[1], context);
        if (b == 0) {
            Logger::log(LOG_WARNING, "DIV", "Division by zero");
            return 0;
        }
        return safeDiv(a, b);
    }
    return 0;
}
bool evalBool(Block *expr, Context &context) {
    if (!expr) return false;
    if (expr->type == Not) {
        if (expr->children.empty()) return false;
        return !evalBool(expr->children[0], context);
    }
    if (expr->type == And) {
        if (expr->children.size() < 2) return false;
        return evalBool(expr->children[0], context) && evalBool(expr->children[1], context);
    }
    if (expr->type == Or) {
        if (expr->children.size() < 2) return false;
        return evalBool(expr->children[0], context) || evalBool(expr->children[1], context);
    }
    switch (expr->type) {
        case Equal:
            if (expr->children.size() < 2) return false;
            return evalValue(expr->children[0], context) == evalValue(expr->children[1], context);
        case Greater:
            if (expr->children.size() < 2) return false;
            return evalValue(expr->children[0], context) > evalValue(expr->children[1], context);
        case Less:
            if (expr->children.size() < 2) return false;
            return evalValue(expr->children[0], context) < evalValue(expr->children[1], context);
        default:
            return false;
    }
}
int evalInt(Block *expr, Context &context) {
    return evalValue(expr, context);
}