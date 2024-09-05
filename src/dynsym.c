#include "dynsym.h"
#include "logger.h"
#include "strtab.h"
#include "symtab.h"

#include <stdlib.h>

#define DSOFFSET(e) (SHDR_M((e), (e)->dynsym_shndx, sh_offset))
#define DSSIZE(e) (SHDR_M((e), (e)->dynsym_shndx, sh_size))
#define DSENTSIZE(e) (SHDR_M((e), (e)->dynsym_shndx, sh_entsize))
#define DSNUM(e) DSSIZE(e) / DSENTSIZE(e)

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

char *getDynsymName(elf_t *elf, int idx, uint32_t name)
{
    int strshndx = SHDR_M(elf, elf->dynsym_shndx, sh_link);

    if (idx >= DSNUM(elf) || name == 0) 
        return "null";
    
    return getStr(elf, strshndx, name);
}

void outputDynsym(elf_t *elf, int idx)
{
    if (idx >= DSNUM(elf)) {
        log_e("idx %d invalid\n", idx);
        return;
    }
    uint32_t name = DYNSYM_M(elf, idx, st_name);
    unsigned char info = DYNSYM_M(elf, idx, st_info);
    unsigned char other = DYNSYM_M(elf, idx, st_other);
    uint16_t shndx = DYNSYM_M(elf, idx, st_shndx);

    printf(" [%03d]    %08x    ", idx, name);
    printf("%-*s", SYMTYPELEN, symType(elf, info));
    printf("%-*s", SYMBINDLEN, symBind(elf, info));
    printf("%-*s", SYMVISLEN, symVis(elf, other));
    switch (shndx) {
        case SHN_ABS:
            printf("%-*s", SYMSHNLEN, "ABS"); break;
        case SHN_COMMON:
            printf("%-*s", SYMSHNLEN, "COMMON"); break;
        case SHN_UNDEF:
            printf("%-*s", SYMSHNLEN, "UNDEF"); break;
        default:
            printf("%04x    ", shndx); break;
    }
    printf("%-16ld    ", DYNSYM_M(elf, idx, st_size));
    printf("%016lx    ", DYNSYM_M(elf, idx, st_value));
    printf("%s", getDynsymName(elf, idx, name));

    printf("\n");
}

void outputDynsymtab(elf_t *elf)
{
    int i;

    if (!elf || elf->symtab_shndx == 0) {
        log_e("elf invalid\n");
        return;
    }

    outputSymTitle();
    
    for (i = 0; i < DSNUM(elf); i++) {
        outputDynsym(elf, i);
    }

}