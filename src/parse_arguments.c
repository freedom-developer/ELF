#include "parse_arguments.h"
#include "global.h"
#include "logger.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static option_t options[] = {
    {0, {"help", no_argument, 0, 0}, OPT_UNDEF, 0, "display this help and exit"},
    {'h', {"header", no_argument, 0, 'h'}, OPT_UNDEF, 0, "output elf's header"},

    /*
    {'p', {"program-header", no_argument, 0, 'p'}, OPT_UNDEF, 0, "print program's header"},
    {'P', {"program", required_argument, 0, 'P'}, OPT_INT, "program-index", "print the specify program, -1 specify all"},
    {'s', {"section-header", no_argument, 0, 's'}, OPT_UNDEF, 0, "print section's header"},
    {'S', {"section", required_argument, 0, 'S'}, OPT_INT, "section-index", "print the specify section, -1 specify all"},
    */

    {0, {0, 0, 0, 0}, 0, 0}
};




static void help(void)
{
    int i;
    size_t options_cnt = sizeof(options) / sizeof(option_t);

    printf("Usage [OPTION ... ] files\n");
    for (i = 0; i < options_cnt; i++) {
        printf("  ");
        if (options[i].short_opt != 0)
            printf("-%c", options[i].short_opt);
        if (options[i].long_opt.name != 0) {
            if (options[i].short_opt != 0)
                printf(", --%s", options[i].long_opt.name);
            else 
                printf("--%s", options[i].long_opt.name);
        }
        if (options[i].long_opt.has_arg == required_argument)
            printf("=%s", options[i].hit);
        else if (options[i].long_opt.has_arg == optional_argument) {
            if (options[i].hit != 0)
                printf("=[%s]", options[i].hit);
        }
        if (options[i].description != 0)
            printf("  %s", options[i].description);
        printf("\n");
    }
}


int parse_arguments(int argc, char **argv)
{
    int i, j, longidx, c;
    struct option *long_opts;
    char *short_opts;
    size_t options_cnt = sizeof(options) / sizeof(option_t);
    size_t sopt_len = options_cnt * 3 + 1;
    size_t long_opts_cnt = options_cnt + 1;
    int exit_flag = 0;

    // 初始化所有的选项参数值
    memset(&optargs, 0, sizeof(optargs));

    short_opts = (char *)malloc(sopt_len);
    memset(short_opts, 0, sopt_len);
    long_opts = (struct option *)calloc(long_opts_cnt, sizeof(struct option));

    for (i = 0, j = 0; i < options_cnt; i++) {
        size_t so_len = strlen(short_opts);
        if (options[i].short_opt != 0) {
            snprintf(short_opts + so_len, sopt_len - so_len, "%c", options[i].short_opt);
            so_len++;
            if (options[i].long_opt.has_arg == required_argument)     
                snprintf(short_opts + so_len, sopt_len - so_len, ":");            
            else if (options[i].long_opt.has_arg == optional_argument) 
                snprintf(short_opts + so_len, sopt_len - so_len, "::");
        }
        if (options[i].long_opt.name != 0) 
            long_opts[j++] = options[i].long_opt;
    }

    while ((c = getopt_long(argc, argv, short_opts, long_opts, &longidx)) != -1) {
        switch(c) {
            case 0:
                if (strcmp(long_opts[longidx].name, "help") == 0) {
                    exit_flag |= 0x80;
                }
                break;
            case 'h':
                optargs.header = 1;
                break;
                /*
            case 'p':
                pr_pheader = 1;
                break;
            case 'P':
                pidx = atoi(optarg);
                break;
            case 's':
                pr_sheader = 1;
                break;
            case 'S':
                sidx = atoi(optarg);
                break;
                */
            case '?':
                exit_flag |= 0x81;
                break;
            default:
                exit_flag |= 0x82;
                break;
        }
    }

    free(short_opts);
    free(long_opts);

out:
    if (exit_flag & 0x80) {
        help();
        exit(exit_flag & 0x7F);
    }

    i = argc - optind;
    if (i > 0) {
        optargs.files = (char **)calloc(sizeof(char *), i + 1);
        if (!optargs.files) {
            fprintf(stderr, "Memory lack\n");
            exit(EXIT_FAILURE);
        }
    } else {
        exit_flag |= 0x83;
        goto out;
    }

    for (j = 0; j < i; j++) {
        optargs.files[j] = argv[optind++];
    }
    
    return 0;
}




