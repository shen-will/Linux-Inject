/*


#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>


#define RTLD_LAZY	0x00001	*/
/* Lazy function call binding.  *//*

#define RTLD_NOW	0x00002	*/
/* Immediate function call binding.  *//*

#define	RTLD_BINDING_MASK   0x3	*/
/* Mask of binding time value.  *//*

#define RTLD_NOLOAD	0x00004	*/
/* Do not load the object.  *//*

#define RTLD_DEEPBIND	0x00008	*/
/* Use deep binding.  *//*


*/
/* If the following bit is set in the MODE argument to `dlopen',
   the symbols of the loaded object and its dependencies are made
   visible as if the object were linked directly into the program.  *//*

#define RTLD_GLOBAL	0x00100


#define __RTLD_DLOPEN	0x80000000


extern void *__libc_dlopen_mode (const char *name, int mode);
extern void *__libc_dlsym (void *map, const char *name);


#define DL_OPEN(name) __libc_dlopen_mode(name,__RTLD_DLOPEN | RTLD_NOW | RTLD_GLOBAL)

#define DL_SYM(handle,name) __libc_dlsym(handle,name)

int main(){

    void *handle = DL_OPEN("./libshen.so");
    if(handle == NULL){
        perror("open so fail");
        return -1;
    }

    void (*fuck)() =(void(*)())DL_SYM(handle,"fuck");
    if(fuck == NULL){
        perror("find method fail");
        return -1;
    }

    fuck();

    return 0;
}*/
