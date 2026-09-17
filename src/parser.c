#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

static const Token *T; static int N, P, errors;
static Token *cur(void){ return (P<N)?(Token*)&T[P]:(Token*)&T[N-1]; }
static int accept(TokenType t){ if(cur()->type==t){P++;return 1;} return 0; }
static int expect(TokenType t,const char *what){ if(accept(t)) return 1; fprintf(stderr,"Parse error (line %d): expected %s, got %s (%s)\n",cur()->line,what,token_type_name(cur()->type),cur()->lexeme); errors++; return 0; }
ASTNode *ast_new(ASTKind k,int line){ ASTNode*n=calloc(1,sizeof(*n)); if(!n)exit(1); n->kind=k;n->line=line;return n; }
static ASTNode *primary(void){ Token*x=cur(); if(accept(TK_ID)){ASTNode*n=ast_new(AST_ID,x->line);strncpy(n->text,x->lexeme,AST_NAME-1);return n;} if(accept(TK_NUM)){ASTNode*n=ast_new(AST_NUM,x->line);n->value=x->value;snprintf(n->text,AST_NAME,"%s",x->lexeme);return n;} if(accept(TK_LPAREN)){ASTNode*n=NULL; n=primary(); /* replaced below by expression parser */ P--; return n;} if(accept(TK_LPAREN)) return NULL; fprintf(stderr,"Parse error (line %d): expected primary, got %s\n",x->line,x->lexeme);errors++;P++;return NULL; }
static ASTNode *expr(void);
static ASTNode *factor(void){ Token*x=cur(); 
    if(accept(TK_OP)&&x->lexeme[0]=='-'){ASTNode*n=ast_new(AST_BINOP,x->line);strcpy(n->text,"neg");n->left=factor();return n;} if(cur()->type==TK_LPAREN){int line=cur()->line;P++;ASTNode*n=expr();expect(TK_RPAREN,")");(void)line;return n;} return primary(); 
}
static ASTNode *term(void){ASTNode*n=factor();while(cur()->type==TK_OP&&(cur()->lexeme[0]=='*'||cur()->lexeme[0]=='/')){Token*x=cur();P++;ASTNode*r=factor();ASTNode*q=ast_new(AST_BINOP,x->line);strncpy(q->text,x->lexeme,AST_NAME-1);q->left=n;q->right=r;n=q;}return n;}
static ASTNode *expr(void){ASTNode*n=term();while(cur()->type==TK_OP&&(cur()->lexeme[0]=='+'||cur()->lexeme[0]=='-')){Token*x=cur();P++;ASTNode*r=term();ASTNode*q=ast_new(AST_BINOP,x->line);strncpy(q->text,x->lexeme,AST_NAME-1);q->left=n;q->right=r;n=q;}return n;}
static ASTNode *condition(void){ASTNode*l=expr();if(cur()->type!=TK_RELOP){fprintf(stderr,"Parse error (line %d): expected relational operator\n",cur()->line);errors++;return l;}Token*x=cur();P++;ASTNode*r=expr();ASTNode*n=ast_new(AST_BINOP,x->line);strncpy(n->text,x->lexeme,AST_NAME-1);n->left=l;n->right=r;return n;}
static ASTNode *stmt(void);
static ASTNode *block(void){Token*x=cur();if(!expect(TK_LBRACE,"{") )return NULL;ASTNode*b=ast_new(AST_BLOCK,x->line),**tail=NULL;ASTNode*head=NULL;tail=&head;while(cur()->type!=TK_RBRACE&&cur()->type!=TK_EOF){ASTNode*s=stmt();if(s){*tail=s;tail=&s->next;}}expect(TK_RBRACE,"}");b->left=head;return b;}
static ASTNode *stmt(void){Token*x=cur();if(cur()->type==TK_LBRACE)return block();if(accept(TK_IF)){ASTNode*n=ast_new(AST_IF,x->line);expect(TK_LPAREN,"(");n->left=condition();expect(TK_RPAREN,")");n->right=stmt();if(accept(TK_ELSE))n->third=stmt();return n;}if(accept(TK_WHILE)){ASTNode*n=ast_new(AST_WHILE,x->line);expect(TK_LPAREN,"(");n->left=condition();expect(TK_RPAREN,")");n->right=stmt();return n;}if(cur()->type==TK_ID){P++;ASTNode*n=ast_new(AST_ASSIGN,x->line);strncpy(n->text,x->lexeme,AST_NAME-1);if(!expect(TK_ASSIGN,"=") ){while(cur()->type!=TK_SEMI&&cur()->type!=TK_EOF)P++;accept(TK_SEMI);return n;}n->left=expr();expect(TK_SEMI,";");return n;}if(cur()->type==TK_SEMI){P++;return ast_new(AST_EMPTY,x->line);}fprintf(stderr,"Parse error (line %d): unexpected token %s\n",x->line,x->lexeme);errors++;while(cur()->type!=TK_SEMI&&cur()->type!=TK_RBRACE&&cur()->type!=TK_EOF)P++;accept(TK_SEMI);return NULL;}
ASTNode *parse(const Token*tokens,int count){T=tokens;N=count;P=0;errors=0;ASTNode*root=ast_new(AST_BLOCK,1),**tail=NULL,*head=NULL;tail=&head;while(cur()->type!=TK_EOF){ASTNode*s=stmt();if(s){*tail=s;tail=&s->next;}}root->left=head;return errors?NULL:root;}
int parser_error_count(void){return errors;}
static void ind(int n){while(n--)printf("  ");}
void dump_ast(const ASTNode*n,int in){for(;n;n=n->next){ind(in);switch(n->kind){case AST_BLOCK:printf("BLOCK\n");dump_ast(n->left,in+1);break;case AST_ASSIGN:printf("ASSIGN %s\n",n->text);dump_ast(n->left,in+1);break;case AST_IF:printf("IF %s\n",n->text);dump_ast(n->left,in+1);ind(in);printf("THEN\n");dump_ast(n->right,in+1);if(n->third){ind(in);printf("ELSE\n");dump_ast(n->third,in+1);}break;case AST_WHILE:printf("WHILE\n");dump_ast(n->left,in+1);dump_ast(n->right,in+1);break;case AST_BINOP:printf("OP %s\n",n->text);dump_ast(n->left,in+1);dump_ast(n->right,in+1);break;case AST_ID:printf("ID %s\n",n->text);break;case AST_NUM:printf("NUM %s\n",n->text);break;default:printf("EMPTY\n");}}}
void ast_free(ASTNode*n){while(n){ASTNode*next=n->next;ASTNode*l=n->left,*r=n->right,*t=n->third;if(n->kind==AST_BLOCK) ast_free(l); else {ast_free(l);ast_free(r);ast_free(t);}free(n);n=next;}}
