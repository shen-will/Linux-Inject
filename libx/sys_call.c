//
// Created by Hasee on 2019/9/6.
//

#include "unistd_32.h"
#include "stdio.h"
#include "stdlib.h"
#include "libc.h"
#include "sys_call.h"

#define SYS_CALL0(sys_num) ({\
     int result =0;\
     __asm("int $0x80":"=a"(result):"a"(sys_num));\
     result;})

#define SYS_CALL1(sys_num,arg1) ({\
     int result =0;\
     __asm("int $0x80":"=a"(result):"a"(sys_num),"b"(arg1));\
     result;})

#define SYS_CALL2(sys_num,arg1,arg2) ({\
     int result =0;\
     __asm("int $0x80":"=a"(result):"a"(sys_num),"b"(arg1),"c"(arg2));\
     result;})

#define SYS_CALL3(sys_num,arg1,arg2,arg3) ({\
     int result =0;\
     __asm("int $0x80":"=a"(result):"a"(sys_num),"b"(arg1),"c"(arg2),"d"(arg3));\
     result;})

#define SYS_CALL4(sys_num,arg1,arg2,arg3,arg4) ({\
     int result =0;\
     __asm("int $0x80":"=a"(result):"a"(sys_num),"b"(arg1),"c"(arg2),"d"(arg3),"S"arg4);\
     result;})

#define SYS_CALL5(sys_num,arg1,arg2,arg3,arg4,arg5) ({\
     int result =0;\
     __asm("int $0x80":"=a"(result):"a"(sys_num),"b"(arg1),"c"(arg2),"d"(arg3),"S"arg4,"D"arg5);\
     result;})

#define SYS_CALL6(sys_num,arg1,arg2,arg3,arg4,arg5,arg6) ({\
     int result =0;\
     __asm volatile("push %%ebp\n\t"\
             "mov %[arg],%%ebp\n\t"\
             "int $0x80\n\t"\
             "pop %%ebp\n\t"\
     :"=a"(result):"a"(sys_num),"b"(arg1),"c"(arg2),"d"(arg3),"S"(arg4),"D"(arg5),[arg]"m"(arg6));\
     result;})

int xopen(const char *path,int flags,int mode){

    CHECK_RETURN(path == NULL,-1);

    return SYS_CALL3(__NR_open,path,flags,mode);
}

int xread(int fd,char *buf, unsigned int size){

    CHECK_RETURN(fd < 0 || buf == NULL || size == 0,-1);

    return SYS_CALL3(__NR_read,fd,buf,size);
}

int xwrite(int fd,const char *buf, unsigned int size){

    CHECK_RETURN(fd < 0 || buf == NULL || size == 0,-1);

    return SYS_CALL3(__NR_write,fd,buf,size);
}

void xclose(int fd){

    CHECK_RETURN(fd < 0,);

    SYS_CALL1(__NR_close,fd);
}

int xget_pid(){
    return SYS_CALL0(__NR_getpid);
}


void* xmmap(void *addr, int len, int prot, int flags,
           int fd, int offset){

    return  (void*)SYS_CALL6(__NR_mmap2,addr,len,prot,flags,fd,offset);
}

int xmunmap(void *addr, unsigned int length){

    CHECK_RETURN(addr == NULL || length == 0,-1);

    return SYS_CALL2(__NR_munmap,addr,length);
}

int xstat(const char *path, struct stat *info) {

    CHECK_RETURN(path == NULL || info == NULL,-1);
    return SYS_CALL2(__NR_stat64,path,info);
}

