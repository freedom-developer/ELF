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
    uint64_t secs_bits;
    int symtab;
    int dynsym0;
    int prghdr;
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

    uint64_t strtab_bits;
    int symtab_shndx, dynsym_shndx, hash_shndx, dynamic_shndx; // 最多只有一个
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
    
    union 
    {
        Elf32_Sym **sym32;
        Elf64_Sym **sym64;
        void **sym;
    } Sym;
#define sym Sym.sym
#define sym32 Sym.sym32
#define sym64 Sym.sym64
#define SYM_M(e, i, m) ((e)->cls == ELFCLASS32 ? (e)->sym32[i]->m : (e)->sym64[i]->m)

    union {
        Elf32_Sym **dynsym32;
        Elf64_Sym **dynsym64;
        void **dynsym;
    } Dynsym;
#define dynsym Dynsym.dynsym
#define dynsym32 Dynsym.dynsym32
#define dynsym64 Dynsym.dynsym64
#define DYNSYM_M(e, i, m) ((e)->cls == ELFCLASS32 ? (e)->dynsym32[i]->m : (e)->dynsym64[i]->m)    

    union {
        Elf32_Phdr **phdr32;
        Elf64_Phdr **phdr64;
        void **phdr;
    } Phdr;
#define phdr    Phdr.phdr
#define phdr32  Phdr.phdr32
#define phdr64  Phdr.phdr64
#define PHDR_M(e, i, m) ((e)->cls == ELFCLASS32 ? (e)->phdr32[i]->m : (e)->phdr64[i]->m)

    struct elf_s *next;

    void (*destroy)(struct elf_s *elf);
    
} elf_t;



// #define EHDR(e) ((e)->class == ELFCLASS32 ? (Elf32_Ehdr *)((e)->Elf_Ehdr) : (Elf64_Ehdr *)((e)->Elf_Ehdr))
// #define EHDR_SIZE(e) ((e)->class == ELFCLASS32 ? sizeof(Elf32_Ehdr) : sizeof(Elf64_Ehdr))


EXTERN elf_t *elf_head;
EXTERN elf_t **elf_tail;

void free_elfs(void);


#endif