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
    int sheader;
    int secs[10];
} optargs_t;

EXTERN optargs_t optargs;

#define ETYPE_LEN   16
typedef struct elf_s {
    char *filename;
    char *map;
    size_t size;

    char cls, byte_order; 
    char etype[ETYPE_LEN];
    union
    {
        Elf32_Ehdr *ehdr32;
        Elf64_Ehdr *ehdr64;
        void *ehdr;
    } Ehdr;
#define ehdr32 Ehdr.ehdr32
#define ehdr64 Ehdr.ehdr64
#define EHDR_M(e, m) ((e)->cls == ELFCLASS32 ? (e)->ehdr32->m : (e)->ehdr64->m)
#define EHDR_SIZE(e) ((e)->cls == ELFCLASS32 ? sizeof(Elf32_Ehdr) : sizeof(Elf64_Ehdr))

    union
    {
        Elf32_Shdr **shdr32;
        Elf64_Shdr **shdr64;  
        void **shdr;
    } Shdr;
#define shdr    Shdr.shdr
#define shdr32  Shdr.shdr32
#define shdr64  Shdr.shdr64
#define SHDR_M(e, i, m) ((e)->cls == ELFCLASS32 ? (e)->shdr32[i]->m : (e)->shdr64[i]->m)

    

    

    struct elf_s *next;

    void (*destroy)(struct elf_s *elf);
    
} elf_t;



// #define EHDR(e) ((e)->class == ELFCLASS32 ? (Elf32_Ehdr *)((e)->Elf_Ehdr) : (Elf64_Ehdr *)((e)->Elf_Ehdr))
// #define EHDR_SIZE(e) ((e)->class == ELFCLASS32 ? sizeof(Elf32_Ehdr) : sizeof(Elf64_Ehdr))


EXTERN elf_t *elf_head;
EXTERN elf_t **elf_tail;

void free_elfs(void);


#endif