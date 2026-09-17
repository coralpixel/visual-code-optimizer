#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "optimize.h"
#include "dag.h"
static void copyq(Quad*d,const Quad*s){*d=*s;}
static int istemp(const char*s){return s[0]=='t'&&s[1]>='0'&&s[1]<='9';}
static int has_use(const Quad*q,int n,const char*name,int from){for(int i=from;i<n;i++)if(!strcmp(q[i].arg1,name)||!strcmp(q[i].arg2,name))return 1;return 0;}
static const char *node_name(DagNode*n){static char buf[64];for(int i=0;i<n->label_count;i++)if(!istemp(n->labels[i]))return n->labels[i];snprintf(buf,sizeof(buf),"t%d",n->id+1);return buf;}
static int emit_node(DagNode*n,Quad*out,int*no,int line){if(!n||n->emitted)return 0;n->emitted=1;if(n->kind==LEAF_VAR||n->kind==LEAF_CONST)return 0;emit_node(n->left,out,no,line);emit_node(n->right,out,no,line);if(*no<MAX_QUADS){snprintf(out[*no].op,8,"%s",n->op);if(n->left->kind==LEAF_CONST)snprintf(out[*no].arg1,32,"%.17g",n->left->const_val);else if(n->left->kind==LEAF_VAR)snprintf(out[*no].arg1,32,"%s",n->left->name);else snprintf(out[*no].arg1,32,"%s",node_name(n->left));if(n->right->kind==LEAF_CONST)snprintf(out[*no].arg2,32,"%.17g",n->right->const_val);else if(n->right->kind==LEAF_VAR)snprintf(out[*no].arg2,32,"%s",n->right->name);else snprintf(out[*no].arg2,32,"%s",node_name(n->right));snprintf(out[*no].result,32,"%s",node_name(n));out[*no].line=line;(*no)++;}return n->id+1;}
static void optimize_assignment_range(const Quad*q,int start,int end,Quad*out,int*no){
    if(start>end)return;
    DAG d; dag_build(q,start,end,&d);
    int live[MAX_DAG_NODES]={0};
    /* User variables are live; compiler temporaries are live if used later in this block. */
    for(int i=0;i<d.count;i++) for(int j=0;j<d.nodes[i]->label_count;j++) {
        const char *lab=d.nodes[i]->labels[j];
        if(!istemp(lab) || has_use(q,end+1,lab,start+1)) live[i]=1;
    }
    /* Mark all ancestors of live nodes. */
    int changed=1;
    while(changed){changed=0;for(int i=0;i<d.count;i++)if(live[i]){DagNode*n=d.nodes[i];if(n->left&&!live[n->left->id]){live[n->left->id]=1;changed=1;}if(n->right&&!live[n->right->id]){live[n->right->id]=1;changed=1;}}}
    for(int i=0;i<d.count;i++) d.nodes[i]->emitted=0;
    for(int i=0;i<d.count;i++) if(live[i]&&d.nodes[i]->kind==INTERIOR) emit_node(d.nodes[i],out,no,q[start].line);
    for(int i=0;i<d.count;i++) if(live[i]&&d.nodes[i]->label_count){
        DagNode*n=d.nodes[i];char src[32];
        if(n->kind==LEAF_CONST)snprintf(src,32,"%.17g",n->const_val);
        else if(n->kind==LEAF_VAR)snprintf(src,32,"%s",n->name);
        else snprintf(src,32,"%s",node_name(n));
        const char *primary=node_name(n);
        for(int j=0;j<n->label_count;j++){
            if(istemp(n->labels[j]) || (n->kind==INTERIOR && !strcmp(n->labels[j],primary))) continue;
            if(*no>=MAX_QUADS)break;
            strcpy(out[*no].op,"=");strcpy(out[*no].arg1,src);out[*no].arg2[0]=0;strcpy(out[*no].result,n->labels[j]);out[*no].line=q[start].line;(*no)++;
        }
    }
    dag_free(&d);
}
static void optimize_block(const Quad*q,int start,int end,Quad*out,int*no){
    int a=start;
    while(a<=end){
        if((!strcmp(q[a].op,"=")||!strcmp(q[a].op,"+")||!strcmp(q[a].op,"-")||!strcmp(q[a].op,"*")||!strcmp(q[a].op,"/")) && q[a].arg1[0]){
            int b=a; while(b+1<=end && (!strcmp(q[b+1].op,"=")||!strcmp(q[b+1].op,"+")||!strcmp(q[b+1].op,"-")||!strcmp(q[b+1].op,"*")||!strcmp(q[b+1].op,"/")) && q[b+1].arg1[0]) b++;
            optimize_assignment_range(q,a,b,out,no); a=b+1;
        } else { copyq(&out[(*no)++],&q[a]); a++; }
    }
}
int optimize_program(const Quad*in,int n,Quad*out,int*after_dag){CFG cfg;build_cfg(in,n,&cfg);int no=0;for(int b=0;b<cfg.count;b++)optimize_block(in,cfg.blocks[b].start,cfg.blocks[b].end,out,&no);if(after_dag)*after_dag=no;return no;}
