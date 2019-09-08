//
// Created by Hasee on 2019/9/6.
//

#include "stdlib.h"
#include "sys_call.h"
#include "stdio.h"
#include "libc.h"

int xatoi(const char *str,unsigned int size,int hex){

    CHECK_RETURN(str == NULL || size == 0,0);

    if(str[0]=='0' && (str[1] == 'x' || str[1] == 'X')){
        str += 2;
        size -= 2;
    }

    CHECK_RETURN(size <= 0,0);

    int result =0;
    int mask =1;
    char ch;
    while(size>0){
        ch = str[--size];
        if(hex){
            if(ch >=48 && ch <= 57)
                result += (ch - 48)*mask;
            else if(ch >= 65 && ch <= 70)
                result += (ch - 55)*mask;
            else if(ch >= 97 && ch <= 102)
                result += (ch - 87)*mask;
            else
                return 0;
            mask *= 16;

        } else{
            CHECK_RETURN(ch < 48 || ch > 57,0);
            result += (ch - 48)*mask;
            mask *= 10;
        }
    }

    return result;
}



int xitoa(unsigned int value, char *buf) {

    CHECK_RETURN(buf == NULL,-1);

    char *p = buf;
    unsigned int mask=1000000000;
    int count =0;
    int ret =0;
    while (1){
        ret = value/mask;

        if(ret > 0 || count >0){
            *buf++ = ret+48;
            value -= ret*mask;
            count++;
        }
        if(value == 0){
            if(mask > 1)
                *buf++ = 48;
            break;
        }

        mask /=10;
    }

    *buf =0;

    return buf - p;
}

int xstrlen(const char *buf) {

    CHECK_RETURN(buf == NULL || buf[0] == 0,0);

    int ret =0;
    __asm("mov $0,%%al\n\t"
            "cld\n\t"
            "mov $0xffffffff,%%ecx\n\t"
            "repne scasb\n\t"
            "sub $0xffffffff,%%ecx\n\t"
            "neg %%ecx\n\t"
            "dec %%ecx\n\t"
            :"=c"(ret):"D"(buf));

    return ret;
}

int xmemcpy(void *dst, void *src, unsigned int size) {

    CHECK_RETURN(dst == NULL || src == NULL || size == 0,0);

    int word = size / 4;
    int byte = size % 4;

    __asm("mov %%eax,%%ecx\n\t"
          "cld\n\t"
          "rep movsl\n\t"
          "mov %%ebx,%%ecx\n\t"
          "rep movsb\n\t"
            ::"a"(word),"b"(byte),"D"(dst),"S"(src));

    return size;
}

void *xmmap2(unsigned int size){

    CHECK_RETURN(size == 0,NULL);

    return xmmap(NULL, size,PROT_READ | PROT_WRITE,MAP_PRIVATE | MAP_ANONYMOUS,
                -1,0);
}

int xstr_index(const char *buf,unsigned int buf_size,const char *str, unsigned int size){

    CHECK_RETURN(buf == NULL || str == NULL ||
    buf_size == 0 || size ==0 || buf_size < size,-1);

    const char *p = buf;
    int count =0;
    while (buf_size >0){

        if(*p == *str){
            count++;
            str++;
        } else{
            str -= count;
            count =0;
        }

        if(count == size)
            break;

        p++;
        buf_size--;
    }

    return count == size ? (p - size) - buf + 1: -1;
}


void* xmmap_exec(unsigned int *size) {

    DEFINE_STR(cmdline,"/proc//exe");

    int pid = xget_pid();
    char pid_str[10];
    int len = xitoa(pid,pid_str);
    CHECK_RETURN(len <= 0,NULL);

    char cmdline_path[11+len];
    xmemcpy(cmdline_path,cmdline,6);
    xmemcpy(cmdline_path+6,pid_str,len);
    xmemcpy(cmdline_path+6+len,cmdline+6,5);

    return xmmap_file_readonly(cmdline_path,size);
}


void* xmmap_file_readonly(const char *path,unsigned int *size) {

    CHECK_RETURN(path == NULL , NULL);

    struct stat info;
    int ret =xstat(path,&info);
    CHECK_RETURN(ret != 0 || info.st_size <=0,NULL);

    int fd = xopen(path,O_RDONLY,00666);
    CHECK_RETURN(fd < 0,NULL);

    void *exe =xmmap(NULL,info.st_size,PROT_READ,MAP_PRIVATE,fd,0);

    xclose(fd);

    if(size != NULL) *size = info.st_size;

    return exe;
}


void* xdlopen(C_RUNTIME *runtime, const char *path){

    CHECK_RETURN(INVALID_RUNTIME(runtime) || path == NULL,NULL);

    DEFINE_STR(dlopen_name,"__libc_dlopen_mode");

    void*(*libc_dl_open)(const char *,int) = lookup_libc_sym(runtime,dlopen_name,18);

    CHECK_RETURN(libc_dl_open == NULL,NULL);

    //dlopen | ldnow | global
    return libc_dl_open(path,0x80000000 | 0x00002 | 0x00100);
}


void* xdlsym(C_RUNTIME *runtime,void *handle,const char *sym){

    CHECK_RETURN(INVALID_RUNTIME(runtime)
    || handle == NULL || sym == NULL,NULL);

    DEFINE_STR(dlsym_name,"__libc_dlsym");

    void*(*libc_dlsym)(void*,const char*) = lookup_libc_sym(runtime,dlsym_name,12);

    CHECK_RETURN(libc_dlsym == NULL,NULL);

    return libc_dlsym(handle,sym);

}

int xdlclose(C_RUNTIME *runtime,void *handle){

    CHECK_RETURN(INVALID_RUNTIME(runtime) || handle == NULL,-1);

    DEFINE_STR(dlclose_name,"__libc_dlclose");

    int (*libc_dlclose)(void*) = lookup_libc_sym(runtime,dlclose_name,14);

    CHECK_RETURN(libc_dlclose == NULL ,-1);

    return libc_dlclose(handle);
}