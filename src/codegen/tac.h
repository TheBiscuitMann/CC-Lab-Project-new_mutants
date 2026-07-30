#ifndef TAC_H
#define TAC_H
#include "../ast/ast.h"

/* Entry point: generate TAC for the whole program by walking the AST. */
void generate_tac(ASTNode* root);

#endif
