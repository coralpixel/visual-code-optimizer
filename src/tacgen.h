#ifndef TACGEN_H
#define TACGEN_H
#include "ast.h"
#include "quad.h"
int generate_tac(const ASTNode *ast, Quad *out);
#endif
