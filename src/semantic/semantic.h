#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "../ast/ast.h"

extern int semantic_errors;
void check_semantics(ASTNode *node);

#endif