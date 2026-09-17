#ifndef DAG_H
#define DAG_H
#include "quad.h"
#define MAX_DAG_NODES 2048
#define MAX_LABELS 32
typedef enum { LEAF_VAR, LEAF_CONST, INTERIOR } NodeKind;
typedef struct DagNode DagNode;
struct DagNode { int id; NodeKind kind; char op[8],name[32]; double const_val; DagNode *left,*right; char labels[MAX_LABELS][32]; int label_count,is_live,emitted; };
typedef struct { DagNode *nodes[MAX_DAG_NODES]; int count; } DAG;
void dag_build(const Quad*q,int start,int end,DAG*d);
void dag_free(DAG*d);
void dump_dag(const DAG*d);
#endif
