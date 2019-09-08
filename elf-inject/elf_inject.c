//
// Created by Hasee on 2019/8/31.
//

#include "elf_inject.h"
#include "elf_comm.h"

extern int text_expand_inject(ElfStruct *elf,InjectInfo *info);
int data_expand_inject(ElfStruct *elf,InjectInfo *info);
int text_padding_inject(ElfStruct *elf,InjectInfo *info);

    INJECT_OPS __ops[INJECT_SIZE]={
        text_expand_inject,
        data_expand_inject,
        text_padding_inject
};

int register_inject_ops(unsigned int type,INJECT_OPS ops){

    CHECK_RETURN(type >= INJECT_SIZE || ops == 0,-1);

    __ops[type] = ops;

    return 0;
}

INJECT_OPS get_inject_ops(unsigned int type){
    CHECK_RETURN(type >= INJECT_SIZE,NULL);
    return __ops[type];
}


int inject(ElfStruct *elf,InjectInfo *info){

    INJECT_OPS  ops = get_inject_ops(info->type);
    CHECK_RETURN(ops == NULL ,-1);

    return ops(elf,info);
}