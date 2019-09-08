//
// Created by Hasee on 2019/9/8.
//

#ifndef ELF_PARSE_ELF_MEM_H
#define ELF_PARSE_ELF_MEM_H

int expand_mem(void *dst, void *src,
               unsigned int src_size , unsigned int size_infos[][2], unsigned int count);

int del_mem(void *dst, void *src,unsigned int size, unsigned int size_infos[][2], unsigned int count);

#endif //ELF_PARSE_ELF_MEM_H
