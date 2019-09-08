//
// Created by Hasee on 2019/9/6.
//

#ifndef LINUX_CODE_STDLIB_H
#define LINUX_CODE_STDLIB_H

#include "libc.h"

#define NULL ((void*)0)

#define CHECK_RETURN(cond,ret) if((cond)) return ret


#define DEFINE_STR(var,str)\
char* var =NULL;\
__asm("jmp __"#var"_jmp \n\t"\
"__"#var"_start: .asciz "#str"\n\t"\
"__"#var"_jmp:call __"#var"_end\n\t"\
"__"#var"_end:pop %%eax\n\t"\
"sub $(__"#var"_end - __"#var"_start),%%eax"\
:"=a"(var):)



int xitoa(unsigned int value,char *buf);
int xatoi(const char *str,unsigned int size,int hex);

int xstrlen(const char *buf);

int xmemcpy(void *dst,void *src, unsigned int size);
void *xmmap2(unsigned int size);

int xstr_index(const char *buf,unsigned int buf_size,const char *str, unsigned int size);

void* xmmap_exec(unsigned int *size);

void* xmmap_file_readonly(const char *path,unsigned int *size);

void* xdlopen(C_RUNTIME *runtime, const char *path);

void* xdlsym(C_RUNTIME *runtime,void *handle,const char *sym);

int xdlclose(C_RUNTIME *runtime,void *handle);

#endif //LINUX_CODE_STDLIB_H
