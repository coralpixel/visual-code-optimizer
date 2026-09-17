#ifndef INTERP_H
#define INTERP_H
#include "quad.h"
#define MAX_VARS 512
typedef struct {char name[32];double value;int used;} Var;
typedef struct {Var vars[MAX_VARS];int count;} Env;
int interpret(const Quad*q,int n,Env*env);
int environments_equal(const Env*a,const Env*b);
void dump_env(const Env*e);
#endif
