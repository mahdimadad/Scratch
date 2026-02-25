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
    if (expr->type == Add) {
        return evalValue(expr->children[0], context)
             + evalValue(expr->children[1], context);
    }

    if (expr->type == Sub) {
        return evalValue(expr->children[0], context)
             - evalValue(expr->children[1], context);
    }

    if (expr->type == Mul) {
        return evalValue(expr->children[0], context)
             * evalValue(expr->children[1], context);
    }

    if (expr->type == Div) {
        int rhs = evalValue(expr->children[1], context);
        if (rhs == 0) {
            Logger::log(LOG_WARNING, "DIV", "Division by zero");
            return 0;
        }
        return evalValue(expr->children[0], context) / rhs;
    }
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
        case Equal: return evalValue(expr->children[0], context) == evalValue(expr->children[1], context);
        case Greater: return evalValue(expr->children[0], context) > evalValue(expr->children[1], context);
        case Less: return evalValue(expr->children[0], context) < evalValue(expr->children[1], context);
        case And: return evalBool(expr->children[0], context) && evalBool(expr->children[1], context);
        case Or: return evalBool(expr->children[0], context) || evalBool(expr->children[1], context);
        case Not: return !evalBool(expr->children[0], context);
        default: return false;
    }
}
int evalInt(Block *expr, Context &context) {
    if (!expr) return 0;
    if (expr->type == NumberLiteral) {
        if (expr->parameters.empty()) return 0;
        return expr->parameters[0];
    }
    if (expr->type == VariableValue) {
        if (expr->text.empty()) return 0;
        return context.variables[expr->text];
    }
    return 0;
}