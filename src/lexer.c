/* ============================================================
 *  lexer.c  -- Hand-written lexical analyzer
 *
 *  Reads the source one character at a time and classifies it.
 *  This is a direct implementation of a DFA: the switch below is
 *  the transition function, and each case is an accepting state.
 * ============================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"

static int g_error_count = 0;

int lexer_error_count(void) { return g_error_count; }

const char *token_type_name(TokenType t)
{
    switch (t) {
        case TK_ID:     return "IDENTIFIER";
        case TK_NUM:    return "NUMBER";
        case TK_OP:     return "OPERATOR";
        case TK_RELOP:  return "RELOP";
        case TK_ASSIGN: return "ASSIGN";
        case TK_LPAREN: return "LPAREN";
        case TK_RPAREN: return "RPAREN";
        case TK_LBRACE: return "LBRACE";
        case TK_RBRACE: return "RBRACE";
        case TK_SEMI:   return "SEMICOLON";
        case TK_IF:     return "KEYWORD_IF";
        case TK_ELSE:   return "KEYWORD_ELSE";
        case TK_WHILE:  return "KEYWORD_WHILE";
        case TK_EOF:    return "EOF";
        default:        return "ERROR";
    }
}

/* Keyword lookup. Keywords are lexically identical to identifiers,
 * so we scan an identifier first and then check it against this
 * table -- the standard approach, and much simpler than encoding
 * every keyword into the DFA itself.                              */
static TokenType keyword_lookup(const char *word)
{
    if (strcmp(word, "if")    == 0) return TK_IF;
    if (strcmp(word, "else")  == 0) return TK_ELSE;
    if (strcmp(word, "while") == 0) return TK_WHILE;
    return TK_ID;
}

/* Helper: append one finished token to the output array. */
static void emit(Token *tokens, int *n, TokenType type,
                 const char *lexeme, double value, int line)
{
    if (*n >= MAX_TOKENS - 1) return;          /* silently stop if full */
    tokens[*n].type  = type;
    tokens[*n].value = value;
    tokens[*n].line  = line;
    strncpy(tokens[*n].lexeme, lexeme, MAX_LEXEME - 1);
    tokens[*n].lexeme[MAX_LEXEME - 1] = '\0';
    (*n)++;
}

int tokenize(const char *src, Token *tokens, int max_tokens)
{
    int i = 0;          /* index into src  */
    int n = 0;          /* tokens produced */
    int line = 1;

    g_error_count = 0;
    (void)max_tokens;   /* MAX_TOKENS is the real bound; kept for clarity */

    while (src[i] != '\0') {
        char c = src[i];

        /* ---- whitespace: skip, but keep the line counter accurate ---- */
        if (c == '\n') { line++; i++; continue; }
        if (isspace((unsigned char)c)) { i++; continue; }

        /* ---- comments: // to end of line ---- */
        if (c == '/' && src[i + 1] == '/') {
            while (src[i] != '\0' && src[i] != '\n') i++;
            continue;
        }

        /* ---- identifiers and keywords:  letter (letter|digit|_)* ---- */
        if (isalpha((unsigned char)c) || c == '_') {
            char buf[MAX_LEXEME];
            int  k = 0;
            while ((isalnum((unsigned char)src[i]) || src[i] == '_')
                   && k < MAX_LEXEME - 1) {
                buf[k++] = src[i++];
            }
            buf[k] = '\0';
            emit(tokens, &n, keyword_lookup(buf), buf, 0, line);
            continue;
        }

        /* ---- numbers:  digit+ ( . digit+ )? ---- */
        if (isdigit((unsigned char)c)) {
            char buf[MAX_LEXEME];
            int  k = 0;
            while (isdigit((unsigned char)src[i]) && k < MAX_LEXEME - 1)
                buf[k++] = src[i++];
            if (src[i] == '.' && isdigit((unsigned char)src[i + 1])) {
                buf[k++] = src[i++];                       /* the dot   */
                while (isdigit((unsigned char)src[i]) && k < MAX_LEXEME - 1)
                    buf[k++] = src[i++];
            }
            buf[k] = '\0';
            emit(tokens, &n, TK_NUM, buf, atof(buf), line);
            continue;
        }

        /* ---- two-character relational operators ---- */
        /* Checked BEFORE single '=' and '<' so that "<=" is not
         * mis-scanned as '<' followed by '='.                    */
        if ((c == '<' || c == '>' || c == '=' || c == '!') && src[i + 1] == '=') {
            char buf[3] = { c, '=', '\0' };
            emit(tokens, &n, TK_RELOP, buf, 0, line);
            i += 2;
            continue;
        }

        /* ---- single-character tokens ---- */
        {
            char buf[2] = { c, '\0' };
            switch (c) {
                case '+': case '-': case '*': case '/':
                    emit(tokens, &n, TK_OP, buf, 0, line);      break;
                case '<': case '>':
                    emit(tokens, &n, TK_RELOP, buf, 0, line);   break;
                case '=':
                    emit(tokens, &n, TK_ASSIGN, buf, 0, line);  break;
                case '(':
                    emit(tokens, &n, TK_LPAREN, buf, 0, line);  break;
                case ')':
                    emit(tokens, &n, TK_RPAREN, buf, 0, line);  break;
                case '{':
                    emit(tokens, &n, TK_LBRACE, buf, 0, line);  break;
                case '}':
                    emit(tokens, &n, TK_RBRACE, buf, 0, line);  break;
                case ';':
                    emit(tokens, &n, TK_SEMI, buf, 0, line);    break;
                default:
                    /* Error recovery: report and keep going, so one bad
                     * character does not hide the rest of the file.   */
                    fprintf(stderr,
                            "Lexical error (line %d): unexpected character '%c'\n",
                            line, c);
                    g_error_count++;
                    emit(tokens, &n, TK_ERROR, buf, 0, line);
                    break;
            }
            i++;
        }
    }

    emit(tokens, &n, TK_EOF, "<eof>", 0, line);
    return n;
}

void dump_tokens(const Token *tokens, int count)
{
    int i;
    printf("--- TOKEN STREAM (%d tokens) ---\n", count);
    for (i = 0; i < count; i++) {
        printf("  <%-14s \"%s\", line %d>\n",
               token_type_name(tokens[i].type),
               tokens[i].lexeme,
               tokens[i].line);
    }
    printf("\n");
}
