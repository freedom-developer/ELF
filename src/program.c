#include "program.h"
#include "logger.h"

#include <stdlib.h>
#include <string.h>

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

#define PHDROFFSET(e)    EHDR_M((e), e_phoff)
#define PRGENTSIZE(e)   EHDR_M((e), e_phentsize)
#define PRGNUM(e)       EHDR_M((e), e_phnum)

int setPhdrs(elf_t *elf)
{
    int i;

    if (!elf) {
        log_er(-1, "elf invalid\n");
    }

    if (PRGNUM(elf) == 0) {
        log_er(-1, "elf have no program\n");
    }

    if (elf->phdr) {
        free(elf->phdr);
        elf->phdr = NULL;
    }

    if (elf->size < PHDROFFSET(elf) + PRGENTSIZE(elf) * PRGNUM(elf)) {
        log_er(-1, "elf size %ld invliad\n", elf->size);
    }

    elf->phdr = (void **)calloc(PRGNUM(elf), sizeof(void *));
    if (!elf->phdr) {
        log_er(-1, "No memory\n");
    }

    for (i = 0; i < PRGNUM(elf); i++) {
        elf->phdr[i] = (void *)(elf->map + PHDROFFSET(elf) + i * PRGENTSIZE(elf));
    }

    return 0;
}

#define PHDRTPLEN   15
#define PHDRFLAGLEN 10
#define PHDROFFLEN  20
#define PHDRADDRLEN 20
#define PHDRSZLEN   20
#define PHDRALNLEN  20

char *ptype(uint32_t type)
{
    switch (type) {
        case PT_NULL:           return "PT_NULL";
        case PT_LOAD:           return "PT_LOAD";
        case PT_DYNAMIC:        return "PT_DYNAMIC";
        case PT_INTERP:         return "PT_INTERP";
        case PT_NOTE:           return "PT_NOTE";
        case PT_SHLIB:          return "PT_SHLIB";
        case PT_PHDR:           return "PT_PHDR";
        case PT_LOPROC:         return "PT_LOPROC";
        case PT_HIPROC:         return "PT_HIPROC";        
        case PT_GNU_STACK:      return "PT_GNU_STACK";
        default:                return "PT_UNKNOWN";
    }
}

char *pflags(uint32_t flags)
{
#define HDRFLAGSBUFLEN 12
    static char hdr_flags[HDRFLAGSBUFLEN];

    memset(hdr_flags, 0, HDRFLAGSBUFLEN);
    snprintf(hdr_flags, HDRFLAGSBUFLEN, "PF_");
    if (flags & PF_R) snprintf(hdr_flags + strlen(hdr_flags), HDRFLAGSBUFLEN - strlen(hdr_flags), "R");
    if (flags & PF_W) snprintf(hdr_flags + strlen(hdr_flags), HDRFLAGSBUFLEN - strlen(hdr_flags), "W");
    if (flags & PF_X) snprintf(hdr_flags + strlen(hdr_flags), HDRFLAGSBUFLEN - strlen(hdr_flags), "X");

    return hdr_flags;
}

void outputPhdrTitle(void)
{
    printf("%-*s", 7, "[Nr]");
    printf("%-*s", PHDRTPLEN, "Type");
    printf("%-*s", PHDRFLAGLEN, "Flags");
    printf("%-*s", PHDROFFLEN, "Offset");
    printf("%-*s", PHDRADDRLEN, "Vaddr");
    printf("%-*s", PHDRADDRLEN, "Paddr");
    printf("%-*s", PHDRSZLEN, "FileSZ");
    printf("%-*s", PHDRSZLEN, "MemSZ");
    printf("%-*s", PHDRALNLEN, "Align");

    printf("\n");
}

void outputPhdr(elf_t *elf, int idx)
{
    if (!elf || !elf->phdr) {
        log_e("elf have no Phdr\n");
        return;
    }

    if (idx >= PRGNUM(elf)) {
        log_e("idx %d invalid\n", idx);
        return;
    }

    printf("[%03d]  ", idx);
    printf("%-*s", PHDRTPLEN, ptype(PHDR_M(elf, idx, p_type)));
    printf("%-*s", PHDRFLAGLEN, pflags(PHDR_M(elf, idx, p_flags)));
    printf("%016lx    ", PHDR_M(elf, idx, p_offset));
    printf("%016lx    ", PHDR_M(elf, idx, p_vaddr));
    printf("%016lx    ", PHDR_M(elf, idx, p_paddr));
    printf("%016lx    ", PHDR_M(elf, idx, p_filesz));
    printf("%016lx    ", PHDR_M(elf, idx, p_memsz));
    printf("%016lx    ", PHDR_M(elf, idx, p_align));
    
    printf("\n");
}

void outputPhdrs(elf_t *elf)
{
    int i;

    if (!elf || !elf->phdr) {
        if (!elf)
            log_e("elf invalid\n");
        else
            log_e("The file %s have no programmer header\n", elf->filename);
        return;
    }

    outputPhdrTitle();

    for (i = 0; i < PRGNUM(elf); i++) {
        outputPhdr(elf, i);
    }

}