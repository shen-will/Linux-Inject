//
// Created by Hasee on 2019/9/8.
//

#include "elf_mem.h"
#include <stdlib.h>
#include <string.h>
#include "elf_comm.h"


#define EXCH_SIZE(array,s1,s2)\
{\
    unsigned int start_s1 = array[s1][0];\
    unsigned int count_s1 = array[s1][1];\
    array[s1][0] = array[s2][0];\
    array[s1][1] = array[s2][1];\
    array[s2][0] = start_s1;\
    array[s2][1] = count_s1;\
}

#define OFFSET(size) (size[0])
#define SIZE(size) (size[1])

int del_mem(void *dst, void *src,unsigned int size, unsigned int size_infos[][2], unsigned int count){

    CHECK_RETURN(dst == NULL || src == NULL|| size == 0 || size_infos == NULL || count == 0,-1);

    //sort asc
    int i=0;
    for(;i<count;i++){

        int k = i;
        for(;k<count;k++){
            if(OFFSET(size_infos[k]) < OFFSET(size_infos[i])){
                EXCH_SIZE(size_infos,i,k);
            }
        }
    }


    int m_dst =0;
    int m_src =0;

    i=0;
    for(;i<count;i++){
        int cp_size = OFFSET(size_infos[i]) - m_src;
        memcpy(dst+m_dst,src+m_src,cp_size);
        m_src += cp_size + SIZE(size_infos[i]);
        m_dst += cp_size;
    }

    if(m_src < size){
        memcpy(dst+m_dst,src+m_src,size - m_src);
    }

    return 0;
}


int expand_mem(void *dst, void *src,
               unsigned int src_size , unsigned int size_infos[][2], unsigned int count){

    CHECK_RETURN(dst == NULL  || src_size == 0 ||
                 src == NULL || size_infos == NULL || count == 0,-1);

    //sort asc
    int i=0;
    for(;i<count;i++){

        int k = i;
        for(;k<count;k++){
            if(OFFSET(size_infos[k]) < OFFSET(size_infos[i])){
                EXCH_SIZE(size_infos,i,k);
            }
        }
    }

    int m_dst =0;
    int m_src =0;

    i=0;
    for(;i<count;i++){
        int cp_size = OFFSET(size_infos[i]) - m_src;
        memcpy(dst+m_dst,src+m_src,cp_size);
        m_dst += cp_size + SIZE(size_infos[i]);
        m_src += cp_size;
    }

    if(m_src < src_size){
        memcpy(dst+m_dst,src+m_src,src_size - m_src);
    }

    return 0;
}