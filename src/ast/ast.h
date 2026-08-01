#ifndef AST_H
#define AST_H


typedef enum {
    NODE_PROGRAM,
    NODE_STATEMENT_LIST,
    NODE_DECLARATION,
    NODE_TYPE,
    NODE_ASSIGNMENT,
    NODE_IF,
    NODE_IF_ELSE,
    NODE_WHILE,
    NODE_PRINT,
    NODE_BLOCK,
    NODE_BINOP,
    NODE_UNOP,
    NODE_IDENTIFIER,
    NODE_INT_LIT,
    NODE_FLOAT_LIT,
    NODE_BOOL_LIT
} NodeType;

// The core structure of the tree
typedef struct ASTNode {
    NodeType type;
    
    // Child pointers for the tree structure
    struct ASTNode *left;
    struct ASTNode *middle;  // Used specifically for if-else (condition, body, else)
    struct ASTNode *right;
    
    // Value fields for leaves (identifiers, numbers, booleans)
    int ival;
    float fval;
    char* sval;
    
    // Operator token (like PLUS, MINUS) for BinOps/UnOps
    int op;
    int line;
} ASTNode;

// Function prototypes so other files can use them
ASTNode* create_node(NodeType type, ASTNode* left, ASTNode* middle, ASTNode* right);
ASTNode* create_leaf_int(int val);
ASTNode* create_leaf_float(float val);
ASTNode* create_leaf_str(NodeType type, char* val);
ASTNode* print_ast(ASTNode* node, int level);

#endif
