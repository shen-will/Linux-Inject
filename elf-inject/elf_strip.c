//
// Created by Hasee on 2019/8/31.
//

#include "elf_strip.h"
#include "elf_inject.h"
#include "elf_mem.h"
#include <string.h>
#include <stdlib.h>

int strip(ElfStruct *elf){

    CHECK_RETURN(INVALID_ELF(elf),-1);

    Elf32_Ehdr *header = read_elf_header(elf->data);

    CHECK_RETURN(INVALID_SH_TAB(header),-1);
    CHECK_RETURN(INVALID_PH_TAB(header),-1);


    unsigned int max_ph_offset =0;

    FOR_EACH_PROGRAM_HEADER(elf->data,ph,{

            int offset = ph->p_offset+ph->p_filesz -1;
            // printf("i %d offset %d\n",i,offset);
            if(offset > max_ph_offset)
            max_ph_offset = offset;
    });

    CHECK_RETURN(max_ph_offset==0,-1);

    unsigned char *bak = (unsigned char*)malloc(elf->size);
   // memcpy(bak,elf->data,elf->size);

    unsigned int size_info[20][2];
    int count=0;

#define APPEND_SIZE(sh)\
if((sh->sh_offset + sh->sh_size) >= max_ph_offset){\
    printf("sh %d--%d may be in segment(max %d) skip\n",sh->sh_offset,sh->sh_size,max_ph_offset);\
}else{\
    size_info[count][0] = sh->sh_offset;\
    size_info[count][1] = sh->sh_size;\
    count++;\
}


    //sh  shstr symbol str  debug
    FOR_EACH_SECION_HEADER(elf->data,sh,{

            //del symbol->
            if(sh->sh_type == SHT_SYMTAB){

                int str_tab_index = sh->sh_link;

                if(str_tab_index > 0){
                    Elf32_Shdr *sym_str_sh =SH_BY_INDEX(elf->data,str_tab_index);
                    APPEND_SIZE(sym_str_sh);
                }

                APPEND_SIZE(sh);
            }else{
                //del debug info
                char *name = read_string_by_offset(elf->data,sh->sh_name);

                //name prefix debug_ may be debug section
                if(strncmp(name,".debug_",7) == 0){
                    APPEND_SIZE(sh);
                }
            }

    });

    //del sh_str
    if(header->e_shstrndx > 0){
        Elf32_Shdr *sh_str = SH_BY_INDEX(elf->data,header->e_shstrndx);
        APPEND_SIZE(sh_str);
    }

#undef APPEND_SIZE
    //del sh_tab
    size_info[count][0] = header->e_shoff;
    size_info[count][1] = header->e_shnum*header->e_shentsize;
    count++;


    int clear =0;
    int i=0;
    for(;i<count;i++){
        printf("start %d size %d\n",size_info[i][0],size_info[i][1]);
        clear+=size_info[i][1];
    }
    printf("clear %d bytes old_len %d pre_new_len %d\n",clear,elf->size,elf->size - clear);

    int ret = del_mem(bak ,elf->data,elf->size,size_info,count);
    CHECK_RETURN(ret < 0,-1);

    printf("new len %d\n",elf->size - clear);

    Elf32_Ehdr *new_header =read_elf_header(bak);

    new_header->e_shnum =0;
    new_header->e_shoff =0;
    new_header->e_shentsize =0;

    free(elf->data);

    elf->data = bak;
    elf->size = elf->size - clear;

    return 0;
}
