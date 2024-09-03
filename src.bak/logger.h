#ifndef _LOGGER_H
#define _LOGGER_H

#define log_i(fmt, ...) fprintf(stdout, fmt, ##__VA_ARGS__)
#define log_e(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__)

#define log_ee(fmt, ...) \
do {\
    log_e(fmt, ##__VA_ARGS);    \
    exit(EXIT_FAILURE); \
} while(0)

#define log_er(ret, fmt, ...) \
do { \
    log_e(fmt, ##__VA_ARGS__);  \
    return(ret);    \
} while (0)

#endif