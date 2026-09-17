#ifndef BLOCK_H
#define BLOCK_H
#include "quad.h"
#define MAX_BLOCKS 1024
typedef struct { int start,end; int succ[2],nsucc; } BasicBlock;
typedef struct { BasicBlock blocks[MAX_BLOCKS]; int count; } CFG;
int build_cfg(const Quad*q,int n,CFG*cfg);
void dump_cfg(const Quad*q,const CFG*cfg);
#endif
