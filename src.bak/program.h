#ifndef _PROGRAM_H
#define _PROGRAM_H

#include <elf.h>
#include <stdio.h>

typedef struct phdr_array_s {
    struct Elf64_Phdr **phdr_array;
    int phnum;
} phdr_array_t;

void parse_program(unsigned char *file_map, size_t filel_size);

#endif