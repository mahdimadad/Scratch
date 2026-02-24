#include "core/Eval.h"
static int safeDiv(int a, int b) {
    if (b == 0) return 0;
    return a / b;
}
int evalValue(Block *expr, Context &context) {
    if (!expr) return 0;
    switch (expr->type) {
        case Number: if (!expr->parameters.empty())return expr->parameters[0];
            return 0;
        case VariableValue: return context.variables[expr->text];
        case Add: return evalValue(expr->children[0], context) + evalValue(expr->children[1], context);
        case Sub: return evalValue(expr->children[0], context) - evalValue(expr->children[1], context);
        case Mul: return evalValue(expr->children[0], context) * evalValue(expr->children[1], context);
        case Div: return safeDiv(evalValue(expr->children[0], context), evalValue(expr->children[1], context));
        default: return 0;
    }
}
bool evalBool(Block *expr, Context &context) {
    if (!expr) return false;
    switch (expr->type) {
        case Equal: return evalValue(expr->children[0], context) == evalValue(expr->children[1], context);
        case Greater: return evalValue(expr->children[0], context) > evalValue(expr->children[1], context);
        case Less: return evalValue(expr->children[0], context) < evalValue(expr->children[1], context);
        case And: return evalBool(expr->children[0], context) && evalBool(expr->children[1], context);
        case Or: return evalBool(expr->children[0], context) || evalBool(expr->children[1], context);
        case Not: return !evalBool(expr->children[0], context);
        default: return false;
    }
}