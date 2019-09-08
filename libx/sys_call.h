//
// Created by Hasee on 2019/9/6.
//

#ifndef LINUX_CODE_SYS_CALL_H
#define LINUX_CODE_SYS_CALL_H

#define PROT_READ	0x1		/* Page can be read.  */
#define PROT_WRITE	0x2		/* Page can be written.  */
#define PROT_EXEC	0x4		/* Page can be executed.  */
#define PROT_NONE	0x0		/* Page can not be accessed.  */

#define MAP_SHARED	0x01		/* Share changes.  */
#define MAP_PRIVATE	0x02		/* Changes are private.  */
#define MAP_FIXED	0x10		/* Interpret addr exactly.  */
#define MAP_ANONYMOUS	0x20

#define MAP_FAILED ((void*)-1)


struct stat {
    unsigned long long	st_dev;
    unsigned char	__pad0[4];

    unsigned long	__st_ino;

    unsigned int	st_mode;
    unsigned int	st_nlink;

    unsigned long	st_uid;
    unsigned long	st_gid;

    unsigned long long	st_rdev;
    unsigned char	__pad3[4];

    long long	st_size;
    unsigned long	st_blksize;

    /* Number 512-byte blocks allocated. */
    unsigned long long	st_blocks;

    unsigned long	st_atime;
    unsigned long	st_atime_nsec;

    unsigned long	st_mtime;
    unsigned int	st_mtime_nsec;

    unsigned long	st_ctime;
    unsigned long	st_ctime_nsec;

    unsigned long long	st_ino;
};

void xclose(int fd);
int xwrite(int fd,const char *buf, unsigned int size);
int xread(int fd,char *buf, unsigned int size);
int xopen(const char *path,int flags,int mode);
int xget_pid();
void* xmmap(void *addr, int len, int prot, int flags,
           int fd, int offset);
int xmunmap(void *addr, unsigned int length);

int xstat(const char *path, struct stat *info);

#endif //LINUX_CODE_SYS_CALL_H
