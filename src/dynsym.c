#include "dynsym.h"

#include <stdlib.h>

#define DSOFFSET(e) (SHDR_M((e), (e)->dynsym_shndx, sh_offset))
#define DSSIZE(e) (SHDR_M((e), (e)->dynsym_shndx, sh_size))
#define DSENTSIZE(e) (SHDR_M((e), (e)->dynsym_shndx, sh_entsize))
#define DSNUM() DSSIZE(e) / DSENTSIZE(e)

int setDynsym(elf_t *elf)
{
    int i;

    if (!elf || !elf->map) {
        log_er(-1, "elf invalid\n");
    }

    if (!elf->dynsym_shndx) {
        log_er(-1, "elf has no dynsym section\n");
    }

    if (elf->size < DSOFFSET(elf) + DSSIZE(elf)) {
        log_er(-1, "elf size %ld invalid\n", elf->size);
    }

    if (elf->dynsym) {
        free(elf->dynsym);
        elf->dynsym = NULL;
    }

    elf->dynsym = (void **)calloc(DSNUM(elf), sizeof(void *));
    if (!elf->dynsym) {
        log_er(-1, "No memory\n");
    }

    for (i = 0; i < DSNUM(elf); i++) {
        elf->dynsym[i] = (void *)(elf->map + DSOFFSET(elf) + i * DSENTSIZE(elf));
    }

    return 0;
}

