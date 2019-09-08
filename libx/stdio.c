//
// Created by Hasee on 2019/9/7.
//



#include "stdio.h"
#include "sys_call.h"
#include "stdlib.h"

int xputs(const char *msg,int size){
    return xwrite(STDOUT_NO,msg,size);
}


int xreadline(unsigned int fd,char *buf){

    CHECK_RETURN(buf == NULL,-1);

    char *p = buf;
    char ch =0;
    while ((xread(fd,&ch,1) == 1)){
        if(ch == '\n')
            break;
        *p++ = ch;
    }
    *p=0;

    return p - buf;
}