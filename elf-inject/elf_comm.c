//
// Created by Hasee on 2019/8/31.
//

#define _IMPL_ELF

#include "elf_comm.h"
#include "elf_struct.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

Elf32_Ehdr* read_elf_header(void *data){

    CHECK_NULL_RETURN(data,NULL);
    return (Elf32_Ehdr*)data;
}



char* read_string_by_offset(void *data,unsigned int offset){

    CHECK_NULL_RETURN(data,NULL);

    Elf32_Ehdr *elf_header = read_elf_header(data);

    int str_tab_index = elf_header->e_shstrndx;
    CHECK_RETURN(str_tab_index >= elf_header->e_shnum,NULL);  //bad elf

    Elf32_Shdr *str_tab_header = (Elf32_Shdr*)(data+elf_header->e_shoff+(str_tab_index* sizeof(Elf32_Shdr)));

    CHECK_RETURN(str_tab_header->sh_type != SHT_STRTAB,NULL);

    CHECK_RETURN(offset >= str_tab_header->sh_size,NULL);

    return (char*)(data+str_tab_header->sh_offset+offset);

}

Elf32_Phdr *get_text_segment(ElfStruct *elf) {

    CHECK_RETURN(INVALID_ELF(elf),NULL);

    FOR_EACH_PROGRAM_HEADER(elf->data,ph,{
        if(ph->p_type == PT_LOAD && (ph->p_flags & PF_X)
        && !(ph->p_flags & PF_W)){
            return  ph;
        }
    });

    return NULL;
}

Elf32_Shdr *get_text_section(ElfStruct *elf) {

    CHECK_RETURN(INVALID_ELF(elf),NULL);

    FOR_EACH_SECION_HEADER(elf->data,sh,{

        if(sh->sh_type == SHT_PROGBITS &&
           strcmp(".text",read_string_by_offset(elf->data,sh->sh_name)) == 0){
            return sh;
        }

    });

    return NULL;
}

Elf32_Phdr *get_data_segment(ElfStruct *elf) {

    CHECK_RETURN(INVALID_ELF(elf),NULL);

    FOR_EACH_PROGRAM_HEADER(elf->data,ph,{

        if(ph->p_type == PT_LOAD && !(ph->p_flags & PF_X)
           && (ph->p_flags & PF_W)){
            return  ph;
        }
    });

    return NULL;
}

Elf32_Shdr *get_section_by_name(ElfStruct *elf, const char *name) {

    CHECK_RETURN(INVALID_ELF(elf) || name == NULL,NULL);

    FOR_EACH_SECION_HEADER(elf->data,sh,{
        char *sh_name = read_string_by_offset(elf->data,sh->sh_name);
        if(sh_name != NULL && strcmp(name,sh_name) == 0)
            return sh;
    });

    return NULL;
}

int get_sym_by_name(ElfStruct *elf,char *names[],Elf32_Sym* syms[], unsigned int size){

    CHECK_RETURN(INVALID_ELF(elf) || names == 0 || size == 0 || syms == NULL,-1);

    Elf32_Shdr *obj_sym_sh = get_section_by_name(elf,".symtab");
    CHECK_RETURN(obj_sym_sh == NULL,-1);

    char *sym_strtab = (char*)(elf->data+((Elf32_Shdr*)(elf->data+
            read_elf_header(elf->data)->e_shoff))[obj_sym_sh->sh_link]
            .sh_offset);
    int sym_count = obj_sym_sh->sh_size / obj_sym_sh->sh_entsize;

    char mask[size];
    memset(mask,0,size);
    int count =0;

    int i=0;
    Elf32_Sym *sym = (Elf32_Sym*)(elf->data + obj_sym_sh->sh_offset);
    for(;i<sym_count;i++,sym++){
        int k =0;
        for(;k<size;k++){
            if(mask[k] == 1)
                continue;

            char *sym_name = sym_strtab+sym->st_name;
            if(strcmp(sym_name,names[k]) == 0){
                syms[k] = sym;
                mask[k] =1;
                count++;
            }
        }
    }

    return count;
}