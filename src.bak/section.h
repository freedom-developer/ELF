#ifndef _SECTION_H
#define _SECTION_H

#include <elf.h>
#include <stdio.h>


// 重定位分节头链表
typedef struct rel_shdr_s {
    struct Elf64_Shdr *shdr;
    struct rel_shdr_s *next;
} rel_shdr_t;

void parse_section_64(unsigned char *file_map, size_t file_size);

#endif