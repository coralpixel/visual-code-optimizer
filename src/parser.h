#ifndef PARSER_H
#define PARSER_H
#include "lexer.h"
#include "ast.h"
ASTNode *parse(const Token *tokens, int count);
int parser_error_count(void);
#endif
