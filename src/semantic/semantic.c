#include <stdio.h>
#include <stdlib.h>
#include "semantic.h"
#include "../symbol_table/symtab.h"

// Recursively walks the tree to check rules
void check_semantics(ASTNode *node) {
    if (node == NULL) return;

    switch(node->type) {
        case NODE_BLOCK:
            // When we hit a block, enter a new scope
            enter_scope();
            check_semantics(node->left); // statement_list
            exit_scope();
            break;

        case NODE_DECLARATION: {
            // node->left is Type, node->middle is Identifier
            char *type_name = node->left->sval;
            char *var_name = node->middle->sval;
            
            if (!insert_symbol(var_name, type_name)) {
                printf("Semantic Error: Variable '%s' is already declared in this scope.\n", var_name);
                exit(1);
            }
            break;
        }

        case NODE_ASSIGNMENT: {
            // node->left is Identifier, node->right is Expression
            char *var_name = node->left->sval;
            Symbol *sym = lookup_symbol(var_name);
            
            if (sym == NULL) {
                printf("Semantic Error: Variable '%s' used before declaration.\n", var_name);
                exit(1);
            }
            // Check the expression on the right side
            check_semantics(node->right);
            break;
        }

        case NODE_IDENTIFIER: {
            // Check if variable exists when used in expressions (like x + 5)
            char *var_name = node->sval;
            if (lookup_symbol(var_name) == NULL) {
                printf("Semantic Error: Undeclared variable '%s'.\n", var_name);
                exit(1);
            }
            break;
        }

        default:
            // For all other nodes (If, While, Print, BinOp), just keep walking down the tree
            check_semantics(node->left);
            check_semantics(node->middle);
            check_semantics(node->right);
            break;
    }
}