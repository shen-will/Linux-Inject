//
// Created by Hasee on 2019/9/6.
//

#ifndef LINUX_CODE_STDIO_H
#define LINUX_CODE_STDIO_H

#define O_RDONLY	00000000
#define O_WRONLY	00000001
#define O_RDWR		00000002

#define STDIN_NO 0
#define STDOUT_NO 1
#define STDERR_NO 2


int xputs(const char *msg,int size);

int xreadline(unsigned int fd,char *buf);
#endif //LINUX_CODE_STDIO_H
