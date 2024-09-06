#include "section.h"
#include "logger.h"
#include "strtab.h"

#include <string.h>

/*
           typedef struct {
               uint32_t   sh_name;
               uint32_t   sh_type;
               uint32_t   sh_flags;
               Elf32_Addr sh_addr;
               Elf32_Off  sh_offset;
               uint32_t   sh_size;
               uint32_t   sh_link;
               uint32_t   sh_info;
               uint32_t   sh_addralign;
               uint32_t   sh_entsize;
           } Elf32_Shdr;

           typedef struct {
               uint32_t   sh_name;
               uint32_t   sh_type;
               uint64_t   sh_flags;
               Elf64_Addr sh_addr;
               Elf64_Off  sh_offset;
               uint64_t   sh_size;
               uint32_t   sh_link;
               uint32_t   sh_info;
               uint64_t   sh_addralign;
               uint64_t   sh_entsize;
           } Elf64_Shdr;

*/

const char *secType(uint32_t sType)
{
    switch (sType) {
        case SHT_NULL:                  return "SHT_NULL";
        case SHT_PROGBITS:              return "SHT_PROGBITS";
        case SHT_SYMTAB:              return "SHT_SYMTAB";
        case SHT_STRTAB:              return "SHT_STRTAB";
        case SHT_RELA:              return "SHT_RELA";
        case SHT_HASH:              return "SHT_HASH";
        case SHT_DYNAMIC:              return "SHT_DYNAMIC";
        case SHT_NOTE:              return "SHT_NOTE";
        case SHT_NOBITS:              return "SHT_NOBITS";
        case SHT_REL:              return "SHT_REL";
        case SHT_SHLIB:              return "SHT_SHLIB";
        case SHT_DYNSYM:              return "SHT_DYNSYM";
        case SHT_LOPROC:              return "SHT_LOPROC";
        case SHT_HIPROC:              return "SHT_HIPROC";
        case SHT_LOUSER:              return "SHT_LOUSER";
        case SHT_HIUSER:              return "SHT_HIUSER";
        default:       return "SHT_UNKNOWN";
    }
}

int setShdr(elf_t *elf)
{
    int i;
    uint16_t shnum = EHDR_M(elf, e_shnum);
    uint64_t shoff = EHDR_M(elf, e_shoff);
    uint16_t shentsize = EHDR_M(elf, e_shentsize);

    if (!elf || elf->size < shoff + shnum * shentsize) {
        log_er(-1, "elf's size %ld invalid\n", elf->size);
    }

    if (elf->shdr) {
        free(elf->shdr);
        elf->shdr = NULL;
    }

    elf->shdr = (void **)calloc(shnum, sizeof(void *));
    if (!elf->shdr) {
        log_er(-1, "No memory\n");
    }

    for (i = 0; i < shnum; i++) {
        elf->shdr[i] = (void *)(elf->map + shoff + shentsize * i);

        switch (SHDR_M(elf, i, sh_type)) {
            case SHT_STRTAB:
                elf->strtab_bits |= (1ULL << i);
                break;
            case SHT_SYMTAB:
                elf->symtab_shndx = i;
                break;
            case SHT_DYNSYM:
                elf->dynsym_shndx = i;
                break;
            case SHT_HASH:
                elf->hash_shndx = i;
                break;
            case SHT_DYNAMIC:
                elf->dynamic_shndx = i;
                break;
        }
        
    }


    return 0;
}

#define B32WID 12
#define B64WID 20

#define NRWID 6
#define NAMEWID 25
#define TYPEWID 15
#define ADDRWID B64WID
#define OFFWID  B64WID
#define SIZEWID B64WID
#define ENTSIZEWID B64WID
#define FLAGSWID 8
#define LINKWID B32WID
#define INFOWID B32WID
#define ALIGNWID B64WID

void outputShdrTitle(void)
{
    //   Address Offset Size EntSize Flags Link Info Align\n");
    printf("Section Header:\n");
    printf("[Nr]  ");
    printf("%-*s", NAMEWID, "Name"); 
    printf("%-*s", TYPEWID, "Type");
    printf("%-*s", ADDRWID, "Address"); 
    printf("%-*s", OFFWID, "Offset");
    printf("%-*s", SIZEWID, "Size");
    printf("%-*s", ENTSIZEWID, "Entsize"); 
    printf("%-*s", FLAGSWID, "Flags");
    printf("%-*s", LINKWID, "Link"); 
    printf("%-*s", INFOWID, "Info");
    printf("%-*s", ALIGNWID, "Align"); 
    printf("\n");
}

void outputShdr(elf_t *elf, int idx)
{
    if (EHDR_M(elf, e_shnum) <= idx) {
        log_e("idx %d invalid\n", idx);
        return;
    }

    char *name = getSecName(elf, SHDR_M(elf, idx, sh_name));
    char *type = (char *)secType(SHDR_M(elf, idx, sh_type));
    uint64_t addr = SHDR_M(elf, idx, sh_addr);
    uint64_t offset = SHDR_M(elf, idx, sh_offset);
    uint64_t size = SHDR_M(elf, idx, sh_size);
    uint64_t entsize = SHDR_M(elf, idx, sh_entsize);
    uint64_t flags = SHDR_M(elf, idx, sh_flags);
    uint32_t link = SHDR_M(elf, idx, sh_link);
    uint32_t info = SHDR_M(elf, idx, sh_info);
    uint64_t addralign = SHDR_M(elf, idx, sh_addralign);

    char sflags[5] = { 0 };
    if (flags & SHF_WRITE) snprintf(sflags + strlen(sflags), 5-strlen(sflags), "W");
    if (flags & SHF_ALLOC) snprintf(sflags + strlen(sflags), 5-strlen(sflags), "A");
    if (flags & SHF_EXECINSTR) snprintf(sflags + strlen(sflags), 5-strlen(sflags), "X");
    if (flags & SHF_MASKPROC) snprintf(sflags + strlen(sflags), 5-strlen(sflags), "M");

    printf("[%02d]  ", idx);
    printf("%-*s", NAMEWID, name);
    printf("%-*s", TYPEWID, type);
    printf("%016lx    ", addr);
    printf("%016lx    ", offset);
    printf("%016lx    ", size);
    printf("%016lx    ", entsize);
    printf("%-*s", FLAGSWID, sflags);
    printf("%08x    ", link);
    printf("%08x    ", info);
    printf("%016lx    ", addralign);

    printf("\n");

}

void outputAllShdr(elf_t *elf)
{
    int i;
    outputShdrTitle();
    for (i = 0; i < EHDR_M(elf, e_shnum); i++) {
        outputShdr(elf, i);
    }
}

void outputSector_gen(elf_t *elf, int shndx)
{
    char *p;
    size_t offset, size;
    int i;

    if (shndx >= EHDR_M(elf, e_shnum)) {
        log_e("shndx %d invalid\n", shndx);
        return;
    }
    offset = SHDR_M(elf, shndx, sh_offset);
    size = SHDR_M(elf, shndx, sh_size);
    if (elf->size < offset + size) {
        log_e("elf file %s invliad\n", elf->filename);
        return;
    }

    p = elf->map + offset;
    
    printf("0000:  ");
    for (i = 0; i < size; i++) {
        if (i > 0 && i % 16 == 0) log_i("\n%04x:  ", i / 16);
        log_i("%02x ", (uint8_t)p[i]);
    }
    if (i % 16) log_i("\n");

}


void outputSector(elf_t *elf, int shndx)
{
    if (!elf || !elf->shdr) {
        log_e("argment invliad\n");
        return;
    }

    if (EHDR_M(elf, e_shnum) < shndx) {
        log_e("shndx %d invalid\n", shndx);
        return;
    }

    printf("SECTION %d: %s\n", shndx, getSecName(elf, SHDR_M(elf, shndx, sh_name)));
    if (SHDR_M(elf, shndx, sh_type) == SHT_STRTAB) {
        outputStrtab(elf, shndx);
    } else {
        outputSector_gen(elf, shndx);
    }
}
