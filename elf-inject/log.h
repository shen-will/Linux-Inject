//
// Created by Hasee on 2019/9/1.
//

#ifndef ELF_PARSE_LOG_H
#define ELF_PARSE_LOG_H

#define LOG_INFO(...) fprintf(stdout,__VA_ARGS__)

#define LOG_ERROR(...) fprintf(stderr,__VA_ARGS__)

#ifdef SHEN_DEBUG

#define LOG_DEBUG(...) fprintf(stdout,__VA_ARGS__)

#else

#define LOG_DEBUG(...)

#endif

#endif //ELF_PARSE_LOG_H
