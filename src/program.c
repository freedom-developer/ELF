#include "program.h"
#include "logger.h"

#include <stdlib.h>

/*
           typedef struct {
               uint32_t   p_type;
               Elf32_Off  p_offset;
               Elf32_Addr p_vaddr;
               Elf32_Addr p_paddr;
               uint32_t   p_filesz;
               uint32_t   p_memsz;
               uint32_t   p_flags;
               uint32_t   p_align;
           } Elf32_Phdr;

           typedef struct {
               uint32_t   p_type;
               uint32_t   p_flags;
               Elf64_Off  p_offset;
               Elf64_Addr p_vaddr;
               Elf64_Addr p_paddr;
               uint64_t   p_filesz;
               uint64_t   p_memsz;
               uint64_t   p_align;
           } Elf64_Phdr;
*/

#define PRGOFFSET(e)    EHDR_M((e), e_phoff)
#define PRGENTSIZE(e)   EHDR_M((e), e_phentsize)
#define PRGNUM(e)       EHDR_M((e), e_phnum)

int setPrg(elf_t *elf)
{
    int i;

    if (!elf) {
        log_er(-1, "elf invalid\n");
    }

    if (PRGNUM(elf) == 0) {
        log_er(-1, "elf have no program\n");
    }

    if (elf->prg) {
        free(elf->prg);
        elf->prg = NULL;
    }

    if (elf->size < PRGOFFSET(elf) + PRGENTSIZE(elf) * PRGNUM(elf)) {
        log_er(-1, "elf size %ld invliad\n", elf->size);
    }

    elf->prg = (void **)calloc(PRGNUM(elf), sizeof(void *));
    if (!elf->prg) {
        log_er(-1, "No memory\n");
    }

    for (i = 0; i < PRGNUM(elf); i++) {
        elf->prg[i] = elf->map + PRGOFFSET(elf) + i * PRGENTSIZE(elf);
    }

    return 0;
}