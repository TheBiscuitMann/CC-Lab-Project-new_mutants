#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "semantic.h"
#include "../symbol_table/symtab.h"
#include "parser.tab.h" // gives us PLUS, MINUS, AND, NOT token numbers

// Counts every error found. parser.y reports this at the end.
int semantic_errors = 0;

// Report an error and keep going, so one run finds every mistake.
static void sem_error(int line, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    printf("Semantic Error at line %d: ", line);
    vprintf(fmt, args);
    printf("\n");
    va_end(args);
    semantic_errors++;
}

// "int" and "float" are the numeric types. "bool" is not.
static int is_numeric(const char *t) {
    return t && (strcmp(t, "int") == 0 || strcmp(t, "float") == 0);
}

// Turns an operator token back into its symbol, for readable messages.
static const char* op_name(int op) {
    switch (op) {
        case PLUS: return "+";
        case MINUS: return "-";
        case TIMES: return "*";
        case DIVIDE: return "/";
        case MOD: return "%";
        case LT: return "<";
        case GT: return ">";
        case LE: return "<=";
        case GE: return ">=";
        case EQ: return "==";
        case NE: return "!=";
        case AND: return "&&";
        case OR: return "||";
        case NOT: return "!";
        default: return "?";
    }
}

// Works out the type of an expression.
static const char* infer_type(ASTNode *node) {
    if (node == NULL) return NULL;
    switch (node->type) {
        case NODE_INT_LIT: return "int";
        case NODE_FLOAT_LIT: return "float";
        case NODE_BOOL_LIT: return "bool";
        case NODE_IDENTIFIER: {
            Symbol *sym = lookup_symbol(node->sval);
            if (sym == NULL) {
                sem_error(node->line, "Undeclared variable '%s'.", node->sval);
                return NULL;
            }
            return sym->type;
        }
        case NODE_UNOP: {
            const char *t = infer_type(node->left);
            if (t == NULL) return NULL;
            if (node->op == NOT) {
                if (strcmp(t, "bool") != 0) {
                    sem_error(node->line, "Operator '!' needs a bool operand, but got '%s'.", t);
                    return NULL;
                }
                return "bool";
            }
            // unary minus
            if (!is_numeric(t)) {
                sem_error(node->line, "Unary '-' needs a numeric operand, but got '%s'.", t);
                return NULL;
            }
            return t;
        }
        case NODE_BINOP: {
            const char *l = infer_type(node->left);
            const char *r = infer_type(node->right);
            if (l == NULL || r == NULL) return NULL;
            switch (node->op) {
                // Arithmetic
                case PLUS: case MINUS: case TIMES: case DIVIDE:
                    if (!is_numeric(l) || !is_numeric(r)) {
                        sem_error(node->line, "Operator '%s' cannot be applied to '%s' and '%s'.", op_name(node->op), l, r);
                        return NULL;
                    }
                    if (strcmp(l, "float") == 0 || strcmp(r, "float") == 0) return "float";
                    return "int";
                // Modulo
                case MOD:
                    if (strcmp(l, "int") != 0 || strcmp(r, "int") != 0) {
                        sem_error(node->line, "Operator '%%' needs int operands, but got '%s' and '%s'.", l, r);
                        return NULL;
                    }
                    return "int";
                // Ordering
                case LT: case GT: case LE: case GE:
                    if (!is_numeric(l) || !is_numeric(r)) {
                        sem_error(node->line, "Operator '%s' needs numeric operands, but got '%s' and '%s'.", op_name(node->op), l, r);
                        return NULL;
                    }
                    return "bool";
                // Equality
                case EQ: case NE:
                    if (!((is_numeric(l) && is_numeric(r)) || (strcmp(l, "bool") == 0 && strcmp(r, "bool") == 0))) {
                        sem_error(node->line, "Operator '%s' cannot compare '%s' with '%s'.", op_name(node->op), l, r);
                        return NULL;
                    }
                    return "bool";
                // Logical
                case AND: case OR:
                    if (strcmp(l, "bool") != 0 || strcmp(r, "bool") != 0) {
                        sem_error(node->line, "Operator '%s' needs bool operands, but got '%s' and '%s'.", op_name(node->op), l, r);
                        return NULL;
                    }
                    return "bool";
                default: return NULL;
            }
        }
        default: return NULL;
    }
}

// Can a value of type 'from' be stored in a variable of type 'to'?
static int is_assignable(const char *to, const char *from) {
    if (to == NULL || from == NULL) return 1;
    if (strcmp(to, from) == 0) return 1;
    if (strcmp(to, "float") == 0 && strcmp(from, "int") == 0) return 1;
    return 0;
}

// Conditions in if and while must be bool
static void check_condition(ASTNode *cond, const char *where) {
    const char *t = infer_type(cond);
    if (t == NULL) return;
    if (strcmp(t, "bool") != 0) {
        sem_error(cond->line, "Condition of '%s' must be bool, but got '%s'.", where, t);
    }
}

void check_semantics(ASTNode *node) {
    if (node == NULL) return;
    switch (node->type) {
        case NODE_BLOCK:
            enter_scope();
            check_semantics(node->left);
            exit_scope();
            break;
        case NODE_DECLARATION: {
            char *type_name = node->left->sval;
            char *var_name = node->middle->sval;
            Symbol *existing = lookup_symbol(var_name);
            if (!insert_symbol(var_name, type_name, node->line)) {
                sem_error(node->line, "Variable '%s' is already declared in this scope (first declared on line %d).", var_name, existing ? existing->line_declared : 0);
            }
            break;
        }
        case NODE_ASSIGNMENT: {
            char *var_name = node->left->sval;
            Symbol *sym = lookup_symbol(var_name);
            if (sym == NULL) {
                sem_error(node->line, "Variable '%s' is used before declaration.", var_name);
                infer_type(node->right);
                break;
            }
            const char *value_type = infer_type(node->right);
            if (!is_assignable(sym->type, value_type)) {
                sem_error(node->line, "Cannot assign a '%s' value to variable '%s' of type '%s' (declared on line %d).", value_type, var_name, sym->type, sym->line_declared);
            }
            break;
        }
        case NODE_IF:
            check_condition(node->left, "if");
            check_semantics(node->middle);
            break;
        case NODE_IF_ELSE:
            check_condition(node->left, "if");
            check_semantics(node->middle);
            check_semantics(node->right);
            break;
        case NODE_WHILE:
            check_condition(node->left, "while");
            check_semantics(node->middle);
            break;
        case NODE_PRINT:
            infer_type(node->left);
            break;
        case NODE_IDENTIFIER:
            if (lookup_symbol(node->sval) == NULL) {
                sem_error(node->line, "Undeclared variable '%s'.", node->sval);
            }
            break;
        default:
            check_semantics(node->left);
            check_semantics(node->middle);
            check_semantics(node->right);
            break;
    }
}