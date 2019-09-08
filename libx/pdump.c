/*
//
// Created by Hasee on 2019/9/5.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>

#define CHECK_THROW(cond,ret,...)\
if((cond)){\
    fprintf(stderr,__VA_ARGS__);\
    return ret;\
}

#define CHECK_ERROR(cond,ret)\
if((cond)){\
    perror("");\
    return ret;\
}

#define READ_LINE(file,buf)({\
char *p = buf;\
if(file != NULL && buf != NULL) {\
    char ch;\
    while(1){\
        ch = getc(file);\
        if(ch == EOF || ch =='\n')\
            break;\
        *p++ = ch;\
    }\
} p - buf;})


#define IS_READABLE(flags) (flags & 0b1)
#define IS_WRITABLE(flags) (flags & 0b10)
#define IS_EXECABLE(flags) (flags & 0b100)
#define IS_PRIVATE(flags) (flags & 0b1000)
#define IS_STACK(flags) (flags & 0b10000)
#define IS_HEAP(flags) (flags & 0b100000)
#define IS_TEXT(flags) (flags & 0b1000000)


typedef struct _item{

    unsigned int start_addr;
    unsigned int end_addr;
    char *map_path;
    unsigned int flags;  //read write exec private stack heap
    struct _item *next; 
}MAP_ITEM;


int get_exe_path(pid_t pid,char *result){

    char path[20];
    char cmdline[256];

    sprintf(path,"/proc/%d/cmdline",pid);

    int fd = open(path,O_CLOEXEC | O_RDONLY);
    CHECK_ERROR(fd < 0,-1);

    int len = read(fd,cmdline,255);
    cmdline[len] =0;
    close(fd);

    if(cmdline[0] == '.'){
        sprintf(path,"/proc/%d/cwd",pid);
        char cwd[256];

        int cwd_len  = readlink(path,cwd,256);
        int path_len = cwd_len + len;

        CHECK_THROW(path_len > 255,-1,"path len(%d) out of max len 255",path_len);

        memcpy(result,cwd,cwd_len);
        memcpy(result+cwd_len,cmdline+1,len);
        result[path_len]=0;
    } else{
        memcpy(result,cmdline,len+1);
    }
}


MAP_ITEM* read_maps(pid_t pid){

    char path[20];
    sprintf(path,"/proc/%d/maps",pid);

    FILE *file = fopen(path,"r");
    CHECK_ERROR(file ==NULL,NULL);

    char buf[256];
    int len;

    MAP_ITEM *head = NULL;
    MAP_ITEM *cur = NULL;

    while ((len = READ_LINE(file,buf)) > 0){
        buf[len] =0;

        MAP_ITEM *item =(MAP_ITEM*)malloc(sizeof(MAP_ITEM));

        char *addr = strtok(buf," ");
        sscanf(addr,"%x-%x",&item->start_addr,&item->end_addr);

        char *mode = strtok(NULL," ");
        if(mode[0] == 'r')
            item->flags |= 0b1;

        if(mode[1]=='w')
            item->flags |= 0b10;

        if(mode[2] =='x')
            item->flags |= 0b100;

        if(mode[3] == 'p')
            item->flags |= 0b1000;

        strtok(NULL," ");
        strtok(NULL," ");
        strtok(NULL," ");

        char *name = strtok(NULL," ");


        if(name != NULL){
            item->map_path = malloc(256);
            strcpy(item->map_path,name);
        } else{
            item->map_path = "--";
        }

        if(strcmp(item->map_path,"stack") == 0)
            item->flags |= 0b100000;

        if(strcmp(item->map_path,"heap") == 0)
            item->flags |= 0b100000;

        char exe[126];
        get_exe_path(pid,exe);

        if(strcmp(exe,item->map_path) == 0)
            item->flags |= 0b1000000;

        if(head == NULL){
            head = item;
            cur = head;
        } else{
            cur->next = item;
            cur = cur->next;
        }
    }

    if(cur != NULL)
        cur->next = NULL;

    fclose(file);

    return head;
}



int dump(pid_t pid,const char *output){

    if(pid <=0 || output == NULL) return -1;


    MAP_ITEM *head = read_maps(pid);

    CHECK_THROW(head == NULL,-1,"read maps fail");

    //copy text segment
    MAP_ITEM *item = head;
    for(;item != NULL;item = item->next){
        if(IS_TEXT(item->flags)){

        }
    }

    return 0;
}



int main(int argc,char **argv){

    if(argc < 3){
        printf("help: pid output");
        return  -1;
    }

    pid_t pid = atoi(argv[1]);


    return dump(pid,argv[2]);
}*/
