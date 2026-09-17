#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#endif
#include "lexer.h"
#include "parser.h"
#include "tacgen.h"
#include "block.h"
#include "dag.h"
#include "optimize.h"
#include "codegen.h"
#include "peephole.h"
#include "interp.h"
#include "viz.h"
#define MAX_SRC 65536
static char *read_file(const char *path){FILE*f=fopen(path,"rb");if(!f){fprintf(stderr,"Error: cannot open '%s'\n",path);return NULL;}fseek(f,0,SEEK_END);long sz=ftell(f);fseek(f,0,SEEK_SET);if(sz<0||sz>=MAX_SRC){fclose(f);return NULL;}char*b=malloc((size_t)sz+1);if(!b){fclose(f);return NULL;}size_t n=fread(b,1,(size_t)sz,f);b[n]='\0';fclose(f);return b;}
static void ensure_out(void){
#ifdef _WIN32
    _mkdir("out");
#else
    mkdir("out",0755);
#endif
}
int main(int argc,char**argv){if(argc<2){fprintf(stderr,"Usage: %s <source-file>\n",argv[0]);return 1;}char*src=read_file(argv[1]);if(!src)return 1;printf("=== Visual Code Optimizer ===\nInput: %s\n\n--- SOURCE ---\n%s\n",argv[1],src);Token*t=malloc(sizeof(Token)*MAX_TOKENS);int nt=tokenize(src,t,MAX_TOKENS);dump_tokens(t,nt);if(lexer_error_count()){free(t);free(src);return 1;}ASTNode*ast=parse(t,nt);if(!ast){fprintf(stderr,"Parsing failed.\n");free(t);free(src);return 1;}printf("--- AST ---\n");dump_ast(ast->left,0);
Quad *q = malloc(sizeof(Quad) * MAX_QUADS);
Quad *dagq = malloc(sizeof(Quad) * MAX_QUADS);
Quad *reg = malloc(sizeof(Quad) * MAX_QUADS);
Quad *ph = malloc(sizeof(Quad) * MAX_QUADS);

if (!q || !dagq || !reg || !ph) {
    fprintf(stderr, "Error: memory allocation failed\n");
    free(q);
    free(dagq);
    free(reg);
    free(ph);
    free(t);
    free(src);
    return 1;
}
int nq=generate_tac(ast,q);
dump_quads(q,nq,"UNOPTIMIZED TAC");
CFG cfg;build_cfg(q,nq,&cfg);
dump_cfg(q,&cfg);ensure_out();
emit_cfg_dot(q,&cfg,"out/cfg_before.dot");
for(int b=0;b<cfg.count;b++){char path[128];DAG d;dag_build(q,cfg.blocks[b].start,cfg.blocks[b].end,&d);printf("--- DAG BEFORE OPTIMIZATION: B%d ---\n",b);dump_dag(&d);snprintf(path,sizeof(path),"out/dag_B%d_before.dot",b);emit_dag_dot(&d,path);dag_free(&d);}int ndag=optimize_program(q,nq,dagq,NULL);dump_quads(dagq,ndag,"AFTER DAG OPTIMIZATION");int nr=regenerate_code(dagq,ndag,reg);int np=peephole_optimize(reg,nr,ph);dump_quads(ph,np,"FINAL AFTER PEEPHOLE");Env e1={0},e2={0};int ok1=interpret(q,nq,&e1),ok2=interpret(ph,np,&e2);printf("--- VERIFICATION ---\nOriginal interpreter: %s\nOptimized interpreter: %s\n",ok1?"PASS":"FAIL",ok2?"PASS":"FAIL");dump_env(&e1);dump_env(&e2);if(ok1&&ok2) printf("Semantic equivalence: %s\n",environments_equal(&e1,&e2)?"PASS":"FAIL"); else if(!ok1&&!ok2) printf("Semantic equivalence: NOT CHECKED (both executions hit a runtime error)\n"); else printf("Semantic equivalence: FAIL\n");printf("\nInstruction counts: before=%d, after DAG=%d, after peephole=%d\n",nq,ndag,np);if(nq)printf("Overall reduction: %.1f%%\n",100.0*(nq-np)/nq);
free(q);
free(dagq);
free(reg);
free(ph);
free(t);
ast_free(ast);
free(src);
return 0;}
