#include <stdio.h>
#include <stdlib.h>
#include "tac.h"
#include "parser.tab.h"   /* token codes: PLUS, MINUS, LT, AND, ... */

/*  Counters for generating fresh temporary and label names  */
static int temp_count = 0;
static int label_count = 0;

/* new_temp(): returns a fresh temporary name like "t1", "t2", ... */
static char* new_temp() {
    char* name = malloc(16);
    sprintf(name, "t%d", ++temp_count);
    return name;
}

/* new_label(): returns a fresh label name like "L1", "L2", ... */
static char* new_label() {
    char* name = malloc(16);
    sprintf(name, "L%d", ++label_count);
    return name;
}

/* op_str(): turn an operator token code into its TAC text symbol. */
static const char* op_str(int op) {
    switch (op) {
        case PLUS:   return "+";
        case MINUS:  return "-";
        case TIMES:  return "*";
        case DIVIDE: return "/";
        case MOD:    return "%";
        case LT:     return "<";
        case GT:     return ">";
        case LE:     return "<=";
        case GE:     return ">=";
        case EQ:     return "==";
        case NE:     return "!=";
        case AND:    return "&&";
        case OR:     return "||";
        case NOT:    return "!";
        default:     return "?";
    }
}

/* Forward declarations (gen_expr and gen_stmt call each other). */
static char* gen_expr(ASTNode* node);
static void  gen_stmt(ASTNode* node);

/* gen_expr: emit code to compute an expression, RETURN its place
   The "place" is a string: a temp name, a variable name, or a literal. */
static char* gen_expr(ASTNode* node) {
    if (!node) return "";

    switch (node->type) {
        case NODE_INT_LIT: {
            char* buf = malloc(16);
            sprintf(buf, "%d", node->ival);
            return buf;
        }
        case NODE_FLOAT_LIT: {
            char* buf = malloc(32);
            sprintf(buf, "%g", node->fval);
            return buf;
        }
        case NODE_BOOL_LIT:
            return node->sval;
        case NODE_IDENTIFIER:
            return node->sval;

        case NODE_BINOP: {
            char* l = gen_expr(node->left);
            char* r = gen_expr(node->right);
            char* t = new_temp();
            printf("%s = %s %s %s\n", t, l, op_str(node->op), r);
            return t;
        }
        case NODE_UNOP: {
            char* v = gen_expr(node->left);
            char* t = new_temp();
            printf("%s = %s %s\n", t, op_str(node->op), v);
            return t;
        }
        default:
            return "";
    }
}

/* gen_stmt: emit code for a statement. Returns absolutelynothing */
static void gen_stmt(ASTNode* node) {
    if (!node) return;

    switch (node->type) {
        case NODE_PROGRAM:
            gen_stmt(node->left);
            break;

        case NODE_STATEMENT_LIST:
            gen_stmt(node->left);
            gen_stmt(node->right);
            break;

        case NODE_BLOCK:
            gen_stmt(node->left);
            break;

        case NODE_DECLARATION:
            break;

        case NODE_ASSIGNMENT: {
            /* left = identifier, right = expression */
            char* rhs = gen_expr(node->right);
            printf("%s = %s\n", node->left->sval, rhs);
            break;
        }

        case NODE_PRINT: {
            char* v = gen_expr(node->left);
            printf("print %s\n", v);
            break;
        }

        case NODE_IF: {
            /* left=cond, middle=then */
            char* cond = gen_expr(node->left);
            char* Lend = new_label();
            printf("ifFalse %s goto %s\n", cond, Lend);
            gen_stmt(node->middle);
            printf("%s:\n", Lend);
            break;
        }

        case NODE_IF_ELSE: {
            /* left=cond, middle=then, right=else */
            char* cond = gen_expr(node->left);
            char* Lelse = new_label();
            char* Lend  = new_label();
            printf("ifFalse %s goto %s\n", cond, Lelse);
            gen_stmt(node->middle);
            printf("goto %s\n", Lend);
            printf("%s:\n", Lelse);
            gen_stmt(node->right);
            printf("%s:\n", Lend);
            break;
        }

        case NODE_WHILE: {
            /* left=cond, middle=body */
            char* Lstart = new_label();
            char* Lend   = new_label();
            printf("%s:\n", Lstart);
            char* cond = gen_expr(node->left);
            printf("ifFalse %s goto %s\n", cond, Lend);
            gen_stmt(node->middle);
            printf("goto %s\n", Lstart);
            printf("%s:\n", Lend);
            break;
        }

        default:
            break;
    }
}

void generate_tac(ASTNode* root) {
    printf("\n--- Three Address Code ---\n");
    gen_stmt(root);
}
