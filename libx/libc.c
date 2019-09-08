//
// Created by Hasee on 2019/9/6.
//

#include "stdlib.h"
#include "libc.h"
#include "sys_call.h"
#include "stdio.h"



C_RUNTIME* create_runtime(){

    int pid =xget_pid();

    CHECK_RETURN(pid <= 0,NULL);

    char pid_str[10];
    xitoa(pid,pid_str);

    DEFINE_STR(path,"/proc//maps");

    int len = xstrlen(pid_str);

    char buf[12+len];

    xmemcpy(buf,path,6);
    xmemcpy(buf+6,pid_str,len);
    xmemcpy(buf+6+len,path+6,6);

    int fd = xopen(buf,O_RDONLY,00666);

    CHECK_RETURN(fd < 0,NULL);

    char line[512];

    DEFINE_STR(libc_key,"/lib/i386-linux-gnu/libc-");

    while ((len = xreadline(fd,line))>0){

        int index = xstr_index(line,len,libc_key,4);

        if(index > 0)
            break;
    }
    xclose(fd);

    CHECK_RETURN(len <=0 ,NULL); // can not find libc.so

    unsigned int libc_start = xatoi(line,8,1);

    CHECK_RETURN(libc_start <= 0,NULL);

    DEFINE_STR(so_suf,".so");

    int path_start = xstr_index(line,len,libc_key,25);

    CHECK_RETURN(path_start <= 0,NULL);

    int path_end = xstr_index(line,len,so_suf,3);
    CHECK_RETURN(path_end <= 0 ,NULL);
    path_end += 3;

    int libc_path_len = path_end - path_start;
    char libc_path[libc_path_len+1];
    libc_path[libc_path_len]=0;

    xmemcpy(libc_path,line+path_start,libc_path_len);

    unsigned int map_size =0;
    void *map = xmmap_file_readonly(libc_path,&map_size);

    CHECK_RETURN(map == NULL || map == MAP_FAILED ,NULL);

    C_RUNTIME *runtime = xmmap2(sizeof(C_RUNTIME));

    if(runtime == NULL || runtime == MAP_FAILED){
        xmunmap(map,map_size);
        return NULL;
    }

    runtime->start_addr = (void*)libc_start;
    runtime->map = map;
    runtime->map_size = map_size;

    runtime->elf_header = (Elf32_Ehdr*)map;

    Elf32_Shdr *sh =(Elf32_Shdr*)(map+runtime->elf_header->e_shoff);

    char *str_tab = (char*)(map+sh[runtime->elf_header->e_shstrndx].sh_offset);

    DEFINE_STR(symtab_name,".dynsym");
    int i=0;
    for(;i<runtime->elf_header->e_shnum;i++,sh++){

        if(sh->sh_type == SHT_DYNSYM && xstr_index(str_tab+sh->sh_name,7,symtab_name,7)==0){
            break;
        }
    }

    if(i == runtime->elf_header->e_shnum){
        free_runtime(runtime);
        return NULL;
    }

    runtime->sym_sh = sh;

    return runtime;
}

void free_runtime(C_RUNTIME *runtime){

    CHECK_RETURN(runtime ==NULL,);

    if(runtime->map != NULL){
        xmunmap(runtime->map,runtime->map_size);
    }

    xmunmap(runtime, sizeof(C_RUNTIME));
}

void* lookup_libc_sym(C_RUNTIME *runtime,const char *name, unsigned  int size){

    CHECK_RETURN(INVALID_RUNTIME(runtime) || name == NULL , NULL);

    char *str_tab = (char*)(runtime->map+((Elf32_Shdr*)(runtime->map+runtime->elf_header->e_shoff))
            [runtime->sym_sh->sh_link].sh_offset);

    int sym_count = runtime->sym_sh->sh_size / runtime->sym_sh->sh_entsize;
    int i=0;
    Elf32_Sym *sym = runtime->map+runtime->sym_sh->sh_offset;
    for(;i<sym_count;i++,sym++){
        if(xstr_index(name,size,str_tab+sym->st_name,size) == 0){
            break;
        }
    }

    CHECK_RETURN(i == sym_count,NULL);


    return runtime->start_addr + sym->st_value;
}


