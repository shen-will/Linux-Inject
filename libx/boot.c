//
// Created by Hasee on 2019/9/7.
//

#include "stdlib.h"
#include "libc.h"
#include "stdio.h"

void show_info(){

    DEFINE_STR(welcome,"bootting....\n");

    xputs(welcome,13);
}

int load_lib(const char *path, const char *startup){

    CHECK_RETURN(path == NULL,-1);

    C_RUNTIME *runtime = create_runtime();

    CHECK_RETURN(INVALID_RUNTIME(runtime),-1);

    void *handle = xdlopen(runtime,path);
    if(handle == NULL){
        free_runtime(runtime);
        return -1;
    }

    if(startup != NULL){
        void (*p_start)(void) = xdlsym(runtime,handle,startup);
        if(p_start == NULL){
            xdlclose(runtime,handle);
            free_runtime(runtime);
            return -1;
        }
        p_start();
    }
    free_runtime(runtime);
    return 0;
}