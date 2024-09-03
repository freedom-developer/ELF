#ifndef _PARSE_ARGUMENTS_H
#define _PARSE_ARGUMENTS_H

#ifdef PARSE_DEFINE_VAR
#define EXTERN 
#else
#define EXTERN extern
#endif

#include <getopt.h>

#define OPT_UNDEF   0
#define OPT_STRING  1
#define OPT_INT     2

typedef struct option_struct {
    char short_opt;         // 0 表示无短选项
    struct option long_opt;
    int opt_type;
    char *hit;              // 提示信息
    char *description;      

} option_t;


int parse_arguments(int argc, char **argv);

#endif
