#ifndef OPTIMIZE_H
#define OPTIMIZE_H
#include "quad.h"
#include "block.h"
int optimize_program(const Quad*in,int n,Quad*out,int *after_dag);
#endif
