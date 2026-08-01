#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../ast/ast.h"
#include "../symbol_table/symtab.h"

void check_semantics(ASTNode *node);

void check_semantics(ASTNode *node) {
    if (node == NULL) {
        return;
    }

    switch (node->type) {
        
        case NODE_PROGRAM:
        case NODE_STATEMENT_LIST:
            check_semantics(node->left);
            check_semantics(node->right);
            break;

        case NODE_ASSIGNMENT: {
            if (node->left != NULL && node->left->type == NODE_IDENTIFIER) {
                char *var_name = node->left->sval;
                
                Symbol *sym = lookup_symbol(var_name);
                if (sym == NULL) {
                    printf("Semantic Error: Undeclared variable '%s'.\n", var_name);
                    exit(1);
                }

                check_semantics(node->right);

                // *** UPDATED TYPE CHECKING ENGINE ***
                // We use strcmp() because sym->type is a string according to symtab.h
                if (strcmp(sym->type, "bool") == 0) {
                    
                    if (node->right->type == NODE_INT_LIT || node->right->type == NODE_BINOP) {
                        printf("Semantic Error: Type mismatch. Cannot assign 'int' to 'bool' variable '%s'.\n", var_name);
                        exit(1);
                    }
                }
            }
            break;
        }

        case NODE_IDENTIFIER: {
            char *var_name = node->sval;
            if (lookup_symbol(var_name) == NULL) {
                printf("Semantic Error: Variable '%s' used before declaration.\n", var_name);
                exit(1);
            }
            break;
        }

        case NODE_BINOP:
            check_semantics(node->left);
            check_semantics(node->right);
            break;

        case NODE_PRINT:
            check_semantics(node->left);
            break;

        case NODE_DECLARATION:
        case NODE_INT_LIT:
            break;

        default:
            if (node->left != NULL) check_semantics(node->left);
            if (node->right != NULL) check_semantics(node->right);
            break;
    }
}