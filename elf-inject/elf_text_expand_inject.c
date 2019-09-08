//
// Created by Hasee on 2019/9/2.
//


#define _IMPL_ELF

#include "elf_struct.h"
#include "elf_comm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "log.h"
#include "elf_inject.h"
#include "elf_mem.h"

#define PAGE_SIZE 0x1000
#define MINI_EXPAND_ADDRESS 10*PAGE_SIZE

#define IS_ALIGN(value,align) ((value % align) ==0)

#define PAGE_ROUND(value) ((value + PAGE_SIZE - 1) & (~(PAGE_SIZE - 1)))

int text_expand_inject(ElfStruct *elf,InjectInfo *info){


    CHECK_RETURN(INVALID_ELF(elf) || INVALID_INJECT_INFO(info),-1);

    ElfStruct *obj = open_elf(info->obj_path);

    Elf32_Phdr *text_segment = get_text_segment(elf);
    CHECK_THROW(text_segment == NULL,-1,"can not found elf text segment\n");

    CHECK_THROW(!IS_ALIGN(text_segment->p_vaddr,PAGE_SIZE),-1,
            "text segment address %p not align page size\n",(void*)text_segment->p_vaddr);

    int inject_size = text_segment->p_vaddr - MINI_EXPAND_ADDRESS;

    CHECK_THROW(inject_size <=0,-1,"max expand size %d not engouh\n",inject_size);

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

            if(sh->sh_size > (inject_size - PAGE_SIZE)){
                LOG_ERROR("text section size %d out of max size %d\n",sh->sh_size,inject_size);
                break;
            }
            obj_text_sh = sh;
            obj_text_size = sh->sh_size;
            break;
        }

    });

    CHECK_THROW(obj_text_sh == NULL,-1,"can not found inject file text section");

    Elf32_Ehdr *header = read_elf_header(elf->data);

    inject_size = PAGE_ROUND(obj_text_size);

    unsigned inject_start= header->e_ehsize;

    //fix offset
    FOR_EACH_SECION_HEADER_AFTER(elf->data,sh,inject_start,{
        sh->sh_offset+= inject_size;
    });

    FOR_EACH_PROGRAM_HEADER_AFTER(elf->data,ph,text_segment->p_offset+1,{
        ph->p_offset+= inject_size;
    });

    text_segment->p_filesz+= inject_size;
    text_segment->p_memsz+= inject_size;

    header->e_shoff+= inject_size;
    header->e_phoff+= inject_size;


    //fill inject info
    Elf32_Sym *syms[2]={NULL,NULL};
    int ret = GET_SYM_BY_NAME(obj,syms,2,"inject_info_start","xboot");
    CHECK_RETURN(ret <= 0 ,-1);

    int inject_info_offset = syms[0]->st_value - obj_text_sh->sh_addr;
    int xboot_offset = syms[1]->st_value - obj_text_sh->sh_addr;

    void *p_inejct = obj->data+obj_text_sh->sh_offset + inject_info_offset;

    *(unsigned int*)(p_inejct+4) = header->e_entry;

    strcpy(p_inejct+8,info->lib_path);
    if(info->start_up != NULL)
        strcpy(p_inejct +8 +256,info->start_up);

    //fix address

    text_segment->p_vaddr -= inject_size;
    text_segment->p_paddr -= inject_size;
    header->e_entry = text_segment->p_vaddr + inject_start +xboot_offset;


    void *bak = malloc(elf->size + inject_size);

    int size_infos[1][2] = {{inject_start,inject_size}};

    ret =expand_mem(bak,elf->data,elf->size,size_infos,1);

    if(ret !=0){
        LOG_ERROR("expand memory %d fail\n",inject_size);
        obj->destroy(obj);
        return -1;
    }

    memcpy(bak+inject_start,obj->data+obj_text_sh->sh_offset,obj_text_size);

    obj->destroy(obj);


    free(elf->data);
    elf->data = bak;
    elf->size+= inject_size;


    LOG_INFO("expand text %d--%d\n",inject_start,inject_size);

    LOG_INFO("new entry %x\n",read_elf_header(elf->data)->e_entry);

    return 0;
}