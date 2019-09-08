//
// Created by Hasee on 2019/8/31.
//

#ifndef ELF_PARSE_ELF_INJECT_H
#define ELF_PARSE_ELF_INJECT_H


#include <stdio.h>
#include "elf.h"
#include "elf_comm.h"
#include "elf_struct.h"


#define INJECT_TEXT_EXPAND 0
#define INJECT_DATA_EXPAND 1
#define INJECT_TEXT_PADDING 2
#define INJECT_SIZE 3

#define INVALID_INJECT_INFO(info) (info == NULL || info->obj_path == NULL || info->lib_path == NULL)

typedef struct {
    const char  *obj_path;
    const char *lib_path;
    const char *start_up;
    unsigned int type;
}InjectInfo;

typedef int(*INJECT_OPS)(ElfStruct*,InjectInfo*);

int register_inject_ops(unsigned int type,INJECT_OPS ops);

int inject(ElfStruct *elf,InjectInfo *info);



#endif //ELF_PARSE_ELF_INJECT_H
