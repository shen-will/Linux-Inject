//
// Created by Hasee on 2019/8/31.
//

#ifndef ELF_PARSE_ELF_COMM_H
#define ELF_PARSE_ELF_COMM_H

#include "elf.h"
#include "elf_struct.h"


#define INVOKE_CHECK_RETURN(op,ret_type,cond,ret_err,perr)({\
ret_type ret = op;\
if(ret cond){\
if(perr) perror("");\
return ret_err;\
}\
ret;})


#define CHECK_RETURN(cond,ret)\
if((cond)) return ret

#define CHECK_NULL_RETURN(item,ret)\
CHECK_RETURN(item == NULL,ret)

#define CHECK_THROW(cond,ret,...)\
if((cond)){\
    fprintf(stderr,__VA_ARGS__);\
    return ret;\
}


#define GET_FILE_SIZE(file)({\
int index = ftell(file);\
fseek(file,0,SEEK_END);\
long len = ftell(file);\
fseek(file,index,SEEK_SET);\
len;})


#define FOR_EACH_PROGRAM_HEADER_AFTER(data,ph,offset,code){\
Elf32_Ehdr *elf_header = read_elf_header(data);\
Elf32_Phdr* ph;\
int i=0;\
for(;(i<elf_header->e_phnum) && (ph=(Elf32_Phdr*)(data+\
        elf_header->e_phoff+i*elf_header->e_phentsize));i++){\
    if(ph->p_offset >= (offset))\
        code\
}\
}\


#define FOR_EACH_SECION_HEADER_AFTER(data,sh,offset,code){\
Elf32_Ehdr *elf_header = read_elf_header(data);\
Elf32_Shdr* sh;\
int i=0;\
for(;(i<elf_header->e_shnum) && (sh=(Elf32_Shdr*)(data+\
        elf_header->e_shoff+i*elf_header->e_shentsize));i++){\
    if(sh->sh_offset >= (offset))\
        code\
}\
}\


#define FOR_EACH_SECION_HEADER(data,sh,code) FOR_EACH_SECION_HEADER_AFTER(data,sh,0,code)

#define FOR_EACH_PROGRAM_HEADER(data,ph,code) FOR_EACH_PROGRAM_HEADER_AFTER(data,ph,0,code)



#define SH_BY_INDEX(data,index)({\
Elf32_Ehdr *elf_header = read_elf_header(data);\
Elf32_Shdr* sh =(Elf32_Shdr*)(data+\
        elf_header->e_shoff+(index)*elf_header->e_shentsize);\
sh;})


#define  INVALID_ELF(elf) (elf == NULL || elf->data == NULL || elf->size == 0)

#define INVALID_SH_TAB(header)\
(header->e_shnum ==0 || \
header->e_shoff ==0 || \
header->e_shentsize ==0)\

#define INVALID_PH_TAB(header)\
(header->e_phnum ==0 || \
header->e_phoff ==0 || \
header->e_phentsize ==0)\


#define SH_AFTER(sh1,sh2) \
((sh1->sh_offset+sh1->sh_size) > (sh2->sh_offset+sh2->sh_size))

#define SH_BEFORE(sh1,sh2) \
((sh1->sh_offset+sh1->sh_size) < (sh2->sh_offset+sh2->sh_size))

#define PH_AFTER(ph1,ph2) \
((ph1->p_offset+ph1->p_filesz) > (ph2->p_offset+ph2->p_filesz))

#define PH_BEFORE(ph1,ph2) \
((ph1->p_offset+ph1->p_filesz) < (ph2->p_offset+ph2->p_filesz))


#define IN_TEXT_SEGMENT(sh,ph)\
(sh->sh_offset >= ph->p_offset && (sh->sh_offset+sh->sh_size) <= \
(ph->p_offset + ph->p_filesz))

#define GET_SYM_BY_NAME(elf,syms,size,...)({\
char *names[]={__VA_ARGS__};\
int ret = get_sym_by_name(elf,names,syms,size);\
ret;})



char* read_string_by_offset(void *data,unsigned int offset);
Elf32_Ehdr* read_elf_header(void *data);

Elf32_Phdr* get_text_segment(ElfStruct *elf);
Elf32_Shdr* get_text_section(ElfStruct *elf);
Elf32_Phdr* get_data_segment(ElfStruct *elf);

Elf32_Shdr* get_section_by_name(ElfStruct *elf,const char *name);

int get_sym_by_name(ElfStruct *elf,char *names[],Elf32_Sym* syms[], unsigned int size);

#endif //ELF_PARSE_ELF_COMM_H
