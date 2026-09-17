/* ============================================================
 *  lexer.h  -- Token definitions and lexical analyzer interface
 *
 *  Stage 1 of the Visual Code Optimizer pipeline.
 *  Source text  -->  stream of Tokens
 * ============================================================ */

#ifndef LEXER_H
#define LEXER_H

#define MAX_LEXEME  32
#define MAX_TOKENS  4096

/* Every distinct category of token the language can contain. */
typedef enum {
    TK_ID,        /* identifier:  a, b, count        */
    TK_NUM,       /* literal:     42, 3.14           */
    TK_OP,        /* + - * /                         */
    TK_RELOP,     /* < > <= >= == !=                 */
    TK_ASSIGN,    /* =                               */
    TK_LPAREN,    /* (                               */
    TK_RPAREN,    /* )                               */
    TK_LBRACE,    /* {                               */
    TK_RBRACE,    /* }                               */
    TK_SEMI,      /* ;                               */
    TK_IF,        /* keyword: if                     */
    TK_ELSE,      /* keyword: else                   */
    TK_WHILE,     /* keyword: while                  */
    TK_EOF,       /* end of input                    */
    TK_ERROR      /* unrecognised character          */
} TokenType;

typedef struct {
    TokenType type;
    char      lexeme[MAX_LEXEME];  /* the exact text that was matched   */
    double    value;               /* numeric value, valid if TK_NUM    */
    int       line;                /* source line, used in diagnostics  */
} Token;

/* Scans the whole source buffer and fills tokens[].
 * Returns the number of tokens written (always ends with TK_EOF).
 * Lexical errors are reported to stderr but do not stop scanning,
 * so several errors can be surfaced in one run.                      */
int  tokenize(const char *src, Token *tokens, int max_tokens);

/* Number of lexical errors seen during the last tokenize() call. */
int  lexer_error_count(void);

/* Human-readable name for a token type -- used by the debug dump. */
const char *token_type_name(TokenType t);

/* Prints the token stream in the "<CLASS, lexeme, line N>" format
 * used in the Phase 1 report. Useful for the Review 2 demo.        */
void dump_tokens(const Token *tokens, int count);

#endif /* LEXER_H */
