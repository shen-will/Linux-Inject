//
// Created by Hasee on 2019/8/31.
//

#ifndef ELF_PARSE_ELF_STRUCT_H
#define ELF_PARSE_ELF_STRUCT_H

typedef struct _ELF{
    void (*destroy)(struct _ELF*);
    int (*save)(struct _ELF*,const char*);
    void *data;
    const char *path;
    unsigned int size;
}ElfStruct;


ElfStruct* open_elf(const char *path);


#endif //ELF_PARSE_ELF_STRUCT_H
