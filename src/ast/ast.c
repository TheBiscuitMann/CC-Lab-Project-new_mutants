#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
extern int yylineno;

// Creates a standard parent node
ASTNode* create_node(NodeType type, ASTNode* left, ASTNode* middle, ASTNode* right) {
    ASTNode* node = (ASTNode*)calloc(1, sizeof(ASTNode));
    node->type = type;
    node->line = yylineno;
    node->left = left;
    node->middle = middle;
    node->right = right;
    node->sval = NULL;
    return node;
}

// Creates an integer leaf node
ASTNode* create_leaf_int(int val) {
    ASTNode* node = create_node(NODE_INT_LIT, NULL, NULL, NULL);
    node->ival = val;
    return node;
}

// Creates a float leaf node
ASTNode* create_leaf_float(float val) {
    ASTNode* node = create_node(NODE_FLOAT_LIT, NULL, NULL, NULL);
    node->fval = val;
    return node;
}

// Creates a string leaf node (used for Identifiers and Type keywords)
ASTNode* create_leaf_str(NodeType type, char* val) {
    ASTNode* node = create_node(type, NULL, NULL, NULL);
    node->sval = strdup(val);
    return node;
}

// Recursively prints the tree with indentation so you can see the logic visually
ASTNode* print_ast(ASTNode* node, int level) {
    if (node == NULL) return NULL;
    
    // Print the indentation
    for (int i = 0; i < level; i++) printf("  ");

    // Print the node details
    switch(node->type) {
        case NODE_PROGRAM: printf("Program\n"); break;
        case NODE_STATEMENT_LIST: printf("StatementList\n"); break;
        case NODE_DECLARATION: printf("Declaration\n"); break;
        case NODE_TYPE: printf("Type (%s)\n", node->sval); break;
        case NODE_ASSIGNMENT: printf("Assignment\n"); break;
        case NODE_IF: printf("If\n"); break;
        case NODE_IF_ELSE: printf("If-Else\n"); break;
        case NODE_WHILE: printf("While\n"); break;
        case NODE_PRINT: printf("Print\n"); break;
        case NODE_BLOCK: printf("Block\n"); break;
        case NODE_BINOP: printf("BinaryOp\n"); break;
        case NODE_UNOP: printf("UnaryOp\n"); break;
        case NODE_IDENTIFIER: printf("Identifier (%s)\n", node->sval); break;
        case NODE_INT_LIT: printf("IntLit (%d)\n", node->ival); break;
        case NODE_FLOAT_LIT: printf("FloatLit (%f)\n", node->fval); break;
        case NODE_BOOL_LIT: printf("BoolLit (%s)\n", node->sval); break;
    }

    // Recursively print children (pushing them in one level deeper)
    print_ast(node->left, level + 1);
    print_ast(node->middle, level + 1);
    print_ast(node->right, level + 1);
    
    return node;
}
