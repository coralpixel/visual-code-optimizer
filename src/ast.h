#ifndef AST_H
#define AST_H
#include "lexer.h"
#define AST_NAME 32

typedef enum { AST_NUM, AST_ID, AST_BINOP, AST_ASSIGN, AST_IF, AST_WHILE, AST_BLOCK, AST_EMPTY } ASTKind;
typedef struct ASTNode ASTNode;
struct ASTNode {
    ASTKind kind;
    int line;
    char text[AST_NAME];
    double value;
    ASTNode *left, *right, *third, *next;
};
ASTNode *ast_new(ASTKind k, int line);
void ast_free(ASTNode *n);
void dump_ast(const ASTNode *n, int indent);
#endif
