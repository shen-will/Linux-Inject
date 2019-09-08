//
// Created by Hasee on 2019/9/6.
//

#ifndef LINUX_CODE_LIBC_H
#define LINUX_CODE_LIBC_H

#include "elf.h"

typedef struct{

    void* start_addr;
    Elf32_Shdr *sym_sh;
    Elf32_Ehdr *elf_header;
    void* map;
    unsigned int map_size;

}C_RUNTIME;

#define INVALID_RUNTIME(runtime) (runtime == NULL || \
runtime->start_addr == NULL || runtime->sym_sh == NULL || runtime->elf_header == NULL || runtime->map == NULL)


void free_runtime(C_RUNTIME *runtime);

void* lookup_libc_sym(C_RUNTIME *runtime,const char *name, unsigned int size);

C_RUNTIME* create_runtime();

#endif //LINUX_CODE_LIBC_H
