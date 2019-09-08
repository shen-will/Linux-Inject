//
// Created by Hasee on 2019/9/2.
//

#define  _IMPL_ELF

#include "elf_comm.h"
#include "elf_struct.h"
#include "elf_inject.h"
#include "elf.h"
#include "log.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "elf_mem.h"

int data_expand_inject(ElfStruct *elf,InjectInfo *info){

    CHECK_RETURN(INVALID_ELF(elf) || INVALID_INJECT_INFO(info),-1);

    Elf32_Phdr *note_segment = NULL;

    Elf32_Phdr *data_segment = NULL;

    Elf32_Phdr *text_segment = NULL;

    FOR_EACH_PROGRAM_HEADER(elf->data,ph,{

        if(ph->p_type == PT_NOTE){
            note_segment = ph;
            continue;
        }

        if(ph->p_type == PT_LOAD && (ph->p_flags & (PF_R | PF_W)) && !(ph->p_flags & PF_X)){
            data_segment = ph;
            continue;
        }

        if(ph->p_type == PT_LOAD && (ph->p_flags & (PF_R | PF_X)) && !(ph->p_flags & PF_W)){
            text_segment = ph;
            continue;
        }
    });

    CHECK_THROW(note_segment == NULL,-1,"elf not have note type segments");
    CHECK_THROW(data_segment == NULL,-1,"elf not have data segment");

    Elf32_Shdr *text_sh =get_text_section(elf);


    ElfStruct *obj =open_elf(info->obj_path);
    CHECK_THROW(obj == NULL,-1,"open inject file fail");

    Elf32_Shdr *obj_text_sh = NULL;
    unsigned int obj_text_size = 0;

    FOR_EACH_SECION_HEADER(obj->data,sh,{

        if(sh->sh_type == SHT_REL || sh->sh_type == SHT_RELA){
            LOG_ERROR("inject file can not have rel section");
            obj->destroy(obj);
            return -1;

        }

        if(sh->sh_type == SHT_PROGBITS &&
           strcmp(".text",read_string_by_offset(obj->data,sh->sh_name)) == 0){

            obj_text_sh = sh;
            obj_text_size = sh->sh_size;
            break;
        }

    });

    //change note to load
    note_segment->p_type = PT_LOAD;
    note_segment->p_filesz = obj_text_size;
    note_segment->p_memsz = obj_text_size;
    note_segment->p_vaddr = data_segment->p_vaddr+data_segment->p_filesz+data_segment->p_align;
    note_segment->p_paddr = note_segment->p_vaddr;
    note_segment->p_offset = data_segment->p_offset+data_segment->p_filesz;
    note_segment->p_align =text_segment->p_align;
    note_segment->p_flags = text_segment->p_flags;

    Elf32_Ehdr *header = read_elf_header(elf->data);
    //fix offset
    unsigned int inject_start = data_segment->p_offset+data_segment->p_filesz;
    unsigned int new_sh_start = header->e_shoff+(header->e_shentsize*header->e_shnum);

    FOR_EACH_SECION_HEADER(elf->data,sh,{

        if(sh->sh_offset > (new_sh_start-1)){
            sh->sh_offset+=obj_text_size+header->e_shentsize;

        }else if(sh->sh_offset > (inject_start -1)){
            sh->sh_offset+= obj_text_size;
        }
    });

    unsigned int new_elf_size = elf->size + obj_text_size + header->e_shentsize;
    void *bak = malloc(new_elf_size);

    unsigned int size_infos[2][2]={{inject_start,obj_text_size},{new_sh_start,header->e_shentsize}};

    int ret = expand_mem(bak,elf->data,elf->size,size_infos,2);

    if(ret != 0){
        LOG_ERROR("expand memory fail");
        obj->destroy(obj);
        return -1;
    }

    memcpy(bak+inject_start,obj->data+obj_text_sh->sh_offset,obj_text_size);

    //copy text sh
    Elf32_Shdr *tmp_sh = (Elf32_Shdr*)alloca(header->e_shentsize);
    memcpy(tmp_sh,text_sh,header->e_shentsize);
    tmp_sh->sh_name+=1;
    tmp_sh->sh_offset = inject_start;
    tmp_sh->sh_addralign = 1;
    tmp_sh->sh_addr = note_segment->p_vaddr;
    tmp_sh->sh_size = note_segment->p_filesz;

    memcpy(bak+obj_text_size+new_sh_start,tmp_sh,header->e_shentsize);

    unsigned int old_entry = header->e_entry;

    header = read_elf_header(bak);
    header->e_shoff += obj_text_size;
    header->e_shnum+=1;

    //fill inject
    Elf32_Sym *syms[2]={NULL,NULL};
    ret = GET_SYM_BY_NAME(obj,syms,2,"inject_info_start","xboot");
    CHECK_RETURN(ret <= 0 ,-1);

    int inject_info_offset = syms[0]->st_value - obj_text_sh->sh_addr;
    int xboot_offset = syms[1]->st_value - obj_text_sh->sh_addr;

    void *p_inject =bak+inject_start + inject_info_offset;
    *(unsigned int*)(p_inject+4) = old_entry;

    strcpy(p_inject+8,info->lib_path);
    if(info->start_up != NULL)
        strcpy(p_inject +8 +256,info->start_up);

    header->e_entry = note_segment->p_vaddr+xboot_offset;

    free(elf->data);
    elf->data = bak;
    elf->size = new_elf_size;
    obj->destroy(obj);


    return 0;
}