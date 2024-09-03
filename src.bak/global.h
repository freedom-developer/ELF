#ifndef _GLOBAL_H
#define _GLOBAL_H

#include <stdio.h>
#include <elf.h>


#ifdef DEFINE_VAR
#define EXTERN
#else
#define EXTERN extern
#endif

typedef struct optargs_s {
    char **files;
    int header;     // 0表示不输出Elf_Ehdr, 非为表示输出Elf_Ehdr
} optargs_t;

EXTERN optargs_t optargs;

typedef struct elf_s {
    char *filename;
    char *map;
    size_t size;

    char cls; 
    union 
    {
        Elf32_Ehdr *ehdr32;
        Elf64_Ehdr *ehdr64;
        void *ehdr;
    } Ehdr;

    struct elf_s *next;
    
} elf_t;

#define EHDR(e) ((e)->class == ELFCLASS32 ? (Elf32_Ehdr *)((e)->Elf_Ehdr) : (Elf64_Ehdr *)((e)->Elf_Ehdr))
#define EHDR_SIZE(e) ((e)->class == ELFCLASS32 ? sizeof(Elf32_Ehdr) : sizeof(Elf64_Ehdr))

EXTERN elf_t *elf_head;
EXTERN elf_t **elf_tail;



#endif