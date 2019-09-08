//
// Created by Hasee on 2019/8/31.
//

#define _IMPL_ELF

#include "elf_comm.h"
#include "elf_struct.h"
#include <stdlib.h>
#include <stdio.h>
#include "elf_inject.h"


void destroy(ElfStruct* elf){

    if(elf == NULL) return;

    if(elf->data != NULL) free(elf->data);

    free(elf);

    elf->data = NULL;
    elf->size =0;
    elf->path =NULL;
}


int save(ElfStruct *elf,const char *path){

    CHECK_RETURN(INVALID_ELF(elf) || path == NULL,-1);

    FILE *file =fopen(path,"wb");
    CHECK_NULL_RETURN(file,-1);

    size_t len =fwrite(elf->data,1,elf->size,file);

    CHECK_RETURN(len != elf->size,-1);

    return 0;
}



ElfStruct* open_elf(const char *path){

    CHECK_NULL_RETURN(path,NULL);

    FILE *file=INVOKE_CHECK_RETURN(fopen(path,"rb"),FILE*,==NULL,NULL,1);

    int len = GET_FILE_SIZE(file);

    void *map = INVOKE_CHECK_RETURN(malloc((size_t)len),void*,==NULL,NULL,1);

    INVOKE_CHECK_RETURN(fread(map,1,(size_t)len,file),int,!=len,NULL,1);

    ElfStruct *ret =(ElfStruct*)malloc(sizeof(ElfStruct));

    ret->save = save;
    ret->destroy = destroy;
    ret->data = map;
    ret->path = path;
    ret->size = len;

    return ret;
}
