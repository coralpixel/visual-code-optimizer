#ifndef QUAD_H
#define QUAD_H
#define MAX_QUADS 8192
#define MAX_TEXT 32
typedef struct { char op[8], arg1[MAX_TEXT], arg2[MAX_TEXT], result[MAX_TEXT]; int line; } Quad;
void dump_quads(const Quad*q,int n,const char*title);
#endif
