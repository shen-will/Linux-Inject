
#include "elf_struct.h"
#include "stdio.h"
#include <stdlib.h>
#include "elf_inject.h"
#include "elf_strip.h"
#include <string.h>


void print_help(){

    printf("-t inject type\n"
           "    t  text expand inject\n"
           "    d  data expand inject\n"
           "    p  text padding inject\n\n"
           "-s  strip sym info\n"
           "-e  elf file path\n"
           "-i  inject file path\n"
           "-l  shared objec file\n"
           "-m  startup method\n"
           "-o  output\n");
}

int main(int argc,char **argv) {


    if(argc <2){
        print_help();
        return 0;
    }

#define NEXT_ARG \
    argc--;\
    argv++;\

    NEXT_ARG

    char is_strip = 0;
    InjectInfo info ={0};

    ElfStruct *elf = NULL;

    char *output = NULL;

    while (argc >0){
        char *cmd = *argv;

        if(strcmp(cmd,"-t") == 0){
            NEXT_ARG
            char *type = *argv;
            if(strcmp(type,"t") == 0)
                info.type = INJECT_TEXT_EXPAND;
            else if(strcmp(type,"d") == 0)
                info.type =INJECT_DATA_EXPAND;
            else if(strcmp(type,"p") == 0)
                info.type =INJECT_TEXT_PADDING;
        } else if(strcmp(cmd,"-s") == 0){
            is_strip =1;
        } else if(strcmp(cmd,"-e") ==0){
            NEXT_ARG
            elf = open_elf(*argv);
        }else if(strcmp(cmd,"-i") == 0){
            NEXT_ARG
            info.obj_path = *argv;
        }else if(strcmp(cmd,"-o") == 0){
            NEXT_ARG
            output =*argv;
        }else if(strcmp(cmd,"-l") == 0){
            NEXT_ARG
            info.lib_path = *argv;
        }else if(strcmp(cmd,"-m") == 0){
            NEXT_ARG
            info.start_up = *argv;
        }

        NEXT_ARG
    }
#undef NEXT_ARG

    if(INVALID_ELF(elf) || output == NULL){
        print_help();
        return 0;
    }


    if(is_strip){
        int ret = strip(elf);
        if(ret == 0)
            elf->save(elf,output);
        elf->destroy(elf);
        return  ret;
    }

    if(INVALID_INJECT_INFO((&info))){
        print_help();
        return 0;
    }

    int ret = inject(elf,&info);

    if(ret == 0)
        elf->save(elf,output);
    else
        return  ret;

    return 0;
}



