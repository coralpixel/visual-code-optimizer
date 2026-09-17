#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "peephole.h"
static int istemp(const char*s){return s[0]=='t'&&s[1]>='0'&&s[1]<='9';}
static int uses(const Quad*q,int n,const char*x,int skip){for(int i=0;i<n;i++)if(i!=skip&&(!strcmp(q[i].arg1,x)||!strcmp(q[i].arg2,x)))return 1;return 0;}
static int isnum(const char*s,double*v){char*e;*v=strtod(s,&e);return e!=s&&*e=='\0';}
static int remove_at(Quad*q,int*n,int i){for(int j=i;j<*n-1;j++)q[j]=q[j+1];(*n)--;return 1;}
static int sweep(Quad*q,int*n){int changed=0;for(int i=0;i<*n;i++){
 if(i+1<*n&&!strcmp(q[i].op,"=")&&strcmp(q[i].arg2,"")&& !strcmp(q[i+1].op,"=")&&!strcmp(q[i+1].arg1,q[i].result)&&!uses(q,*n,q[i].result,i+1)&&istemp(q[i].result)){strcpy(q[i+1].arg1,q[i].arg1);strcpy(q[i+1].arg2,q[i].arg2);strcpy(q[i+1].op,q[i].op);remove_at(q,n,i);changed=1; i--;continue;}
 if(!strcmp(q[i].op,"=")&&!strcmp(q[i].arg2,"")&&i+1<*n&&!strcmp(q[i+1].op,"=")&&!strcmp(q[i+1].arg1,q[i].result)&&istemp(q[i].result)){strcpy(q[i+1].arg1,q[i].arg1);remove_at(q,n,i);changed=1;i--;continue;}
 if(!strcmp(q[i].op,"+")||!strcmp(q[i].op,"-")||!strcmp(q[i].op,"*")||!strcmp(q[i].op,"/")){double a,b;if(!strcmp(q[i].op,"+")&&(!strcmp(q[i].arg2,"0"))){strcpy(q[i].op,"=");q[i].arg2[0]=0;changed=1;}else if(!strcmp(q[i].op,"*")&&!strcmp(q[i].arg2,"1")){strcpy(q[i].op,"=");q[i].arg2[0]=0;changed=1;}else if(!strcmp(q[i].op,"+")&&(!strcmp(q[i].arg1,"0"))){strcpy(q[i].op,"=");strcpy(q[i].arg1,q[i].arg2);q[i].arg2[0]=0;changed=1;}else if(!strcmp(q[i].op,"*")&&(!strcmp(q[i].arg2,"0"))){strcpy(q[i].op,"=");strcpy(q[i].arg1,"0");q[i].arg2[0]=0;changed=1;}else if(!strcmp(q[i].op,"+")&&isnum(q[i].arg1,&a)&&isnum(q[i].arg2,&b)){char buf[32];snprintf(buf,32,"%.17g",a+b);strcpy(q[i].op,"=");strcpy(q[i].arg1,buf);q[i].arg2[0]=0;changed=1;}else if(!strcmp(q[i].op,"*")&&isnum(q[i].arg1,&a)&&isnum(q[i].arg2,&b)){char buf[32];snprintf(buf,32,"%.17g",a*b);strcpy(q[i].op,"=");strcpy(q[i].arg1,buf);q[i].arg2[0]=0;changed=1;}else if(!strcmp(q[i].op,"-")&&isnum(q[i].arg1,&a)&&isnum(q[i].arg2,&b)){char buf[32];snprintf(buf,32,"%.17g",a-b);strcpy(q[i].op,"=");strcpy(q[i].arg1,buf);q[i].arg2[0]=0;changed=1;}else if(!strcmp(q[i].op,"/")&&isnum(q[i].arg1,&a)&&isnum(q[i].arg2,&b)&&b!=0){char buf[32];snprintf(buf,32,"%.17g",a/b);strcpy(q[i].op,"=");strcpy(q[i].arg1,buf);q[i].arg2[0]=0;changed=1;} }
 if(!strcmp(q[i].op,"*")&&!strcmp(q[i].arg2,"2")){strcpy(q[i].op,"<<");strcpy(q[i].arg2,"1");changed=1;}else if(!strcmp(q[i].op,"/")&&!strcmp(q[i].arg2,"2")){strcpy(q[i].op,">>");strcpy(q[i].arg2,"1");changed=1;}
 }
 return changed;}
int peephole_optimize(const Quad*in,int n,Quad*out){Quad*q=malloc(sizeof(Quad)*MAX_QUADS);for(int i=0;i<n;i++)q[i]=in[i];int m=n;for(int it=0;it<100;it++)if(!sweep(q,&m))break;for(int i=0;i<m;i++)out[i]=q[i];free(q);return m;}
