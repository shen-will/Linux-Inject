//
// Created by Hasee on 2019/9/1.
//

#include "elf_inject.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "elf_comm.h"
#include "log.h"


//inject code to text-data margin


int text_padding_inject(ElfStruct *elf,InjectInfo *info){

    CHECK_RETURN(INVALID_ELF(elf) || INVALID_INJECT_INFO(info),-1);

    Elf32_Shdr *last_text_sh =NULL;

    Elf32_Phdr *text_segment = get_text_segment(elf);
    Elf32_Phdr *data_segment = get_data_segment(elf);

    CHECK_THROW(text_segment == NULL,-1,
                "can not found text segment,the file  has been modified?");

    CHECK_THROW(data_segment == NULL,-1,
                "can not found data segment,the file  has been modified?");

    unsigned  int inject_start = text_segment->p_offset+text_segment->p_filesz;
    unsigned  int inject_size = data_segment->p_offset - inject_start;

    LOG_INFO("max inject size %d\n",inject_size);

    //inject size
    ElfStruct *obj = open_elf(info->obj_path);

    Elf32_Shdr *obj_text_sh = NULL;
    unsigned int obj_text_size =0;

    FOR_EACH_SECION_HEADER(obj->data,sh,{

        if(sh->sh_type == SHT_REL || sh->sh_type == SHT_RELA){
            LOG_ERROR("inject file can not have rel section");
            obj->destroy(obj);
            return -1;

        }

        if(sh->sh_type == SHT_PROGBITS &&
           strcmp(".text",read_string_by_offset(obj->data,sh->sh_name)) == 0){

            if(sh->sh_size > inject_size){
                LOG_ERROR("text section size %d out of max size %d\n",sh->sh_size,inject_size);
                break;
            }
            obj_text_sh = sh;
            obj_text_size = sh->sh_size;
            break;
        }

    });

    if(obj_text_sh==NULL){
        LOG_ERROR("can nont found inject file text seciton");
        obj->destroy(obj);
        return  -1;
    }

    memcpy(elf->data+inject_start,obj->data+obj_text_sh->sh_offset,obj_text_sh->sh_size);
    obj->destroy(obj);

    //fill inject info
    Elf32_Sym *syms[2]={NULL,NULL};
    int ret = GET_SYM_BY_NAME(obj,syms,2,"inject_info_start","xboot");
    CHECK_RETURN(ret <= 0 ,-1);

    int inject_info_offset = syms[0]->st_value - obj_text_sh->sh_addr;
    int xboot_offset = syms[1]->st_value - obj_text_sh->sh_addr;

    void *p_inject_info = elf->data+inject_start + inject_info_offset;

    unsigned int inject_info_size = *(unsigned int*)(p_inject_info);
    *(unsigned int*)(p_inject_info+4) = read_elf_header(elf->data)->e_entry;
    strcpy(p_inject_info+8,info->lib_path);

    if(info->start_up != NULL)
        strcpy(p_inject_info +8 +256,info->start_up);


    //fix offset
    FOR_EACH_SECION_HEADER(elf->data,sh,{

        if(IN_TEXT_SEGMENT(sh,text_segment)){

            last_text_sh = last_text_sh == NULL ? sh :
                    SH_AFTER(sh,last_text_sh) ? sh : last_text_sh;
        }
    });

    CHECK_THROW(last_text_sh == NULL,-1,"find text segment last section error");

    LOG_INFO("append %d bytes to section %s\n",obj_text_size,
            read_string_by_offset(elf->data,last_text_sh->sh_name));

    text_segment->p_filesz+= obj_text_size;
    text_segment->p_memsz+=obj_text_size;

    read_elf_header(elf->data)->e_entry = last_text_sh->sh_addr+last_text_sh->sh_size+xboot_offset;

    LOG_INFO("new entry address %p(%p+%#x)\n",read_elf_header(elf->data)->e_entry,
             last_text_sh->sh_addr+last_text_sh->sh_size,inject_info_size);

    last_text_sh->sh_size+= obj_text_size;


    return 0;

}
