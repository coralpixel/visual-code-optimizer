#ifndef VIZ_H
#define VIZ_H
#include "quad.h"
#include "block.h"
#include "dag.h"
int emit_cfg_dot(const Quad*q,const CFG*cfg,const char*path);
int emit_dag_dot(const DAG*d,const char*path);
#endif
