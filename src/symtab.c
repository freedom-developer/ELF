#include "symtab.h"
#include "logger.h"
#include "strtab.h"

#include <string.h>
#include <stdlib.h>

/*
           typedef struct {
               uint32_t      st_name;
               Elf32_Addr    st_value;
               uint32_t      st_size;
               unsigned char st_info;
               unsigned char st_other;
               uint16_t      st_shndx;
           } Elf32_Sym;

           typedef struct {
               uint32_t      st_name;
               unsigned char st_info;
               unsigned char st_other;
               uint16_t      st_shndx;
               Elf64_Addr    st_value;
               uint64_t      st_size;
           } Elf64_Sym;

*/

#define SYMENTSIZE(e) SHDR_M((e), (e)->symtab_shndx, sh_entsize)
#define SYMNUM(e) (SHDR_M((e), (e)->symtab_shndx, sh_size) / SYMENTSIZE(e))
#define SYMTAGOFFSET(e) SHDR_M((e), (e)->symtab_shndx, sh_offset)


int setSym(elf_t *elf)
{
    if (!elf || !elf->shdr) {
        log_er(-1, "elf invalid\n");
    }

    if (!elf->symtab_shndx) {
        log_er(-1, "elf has no symtab\n");
    }

    int i;
    uint64_t offset = SYMTAGOFFSET(elf);
    uint64_t entsize = SYMENTSIZE(elf);
    uint64_t symnum = SYMNUM(elf);

    elf->sym = (void **)calloc(symnum, sizeof(void *));
    if (!elf->sym) {
        log_er(-1, "No memory\n");
    }

    for (i = 0; i < symnum; i++) {
        elf->sym[i] = (void *)(elf->map + offset + i * entsize);
    }

    return 0;
}

#define SYMNAMELEN 12
#define SYMTYPELEN 15
#define SYMBINDLEN 20
#define SYMVISLEN 15
#define SYMSHNLEN 8
#define SYMSIZELEN 20
#define SYMVALLEN 20

void outputSymTitle(void)
{
    printf(" [ NR]    "); // Name type bind visibility shndx size value");
    printf("%-*s", SYMNAMELEN, "Name");
    printf("%-*s", SYMTYPELEN, "Type");
    printf("%-*s", SYMBINDLEN, "Bind");
    printf("%-*s", SYMVISLEN, "Vis");
    printf("%-*s", SYMSHNLEN, "Shndx");
    printf("%-*s",  SYMSIZELEN, "Size");
    printf("%-*s", SYMVALLEN, "Value");

    printf("\n");
}

char *symType(elf_t *elf, unsigned char info)
{
    switch(elf->cls == ELFCLASS32 ? ELF32_ST_TYPE(info) : ELF64_ST_TYPE(info)) {
        case STT_NOTYPE: return "STT_NOTYPE";
        case STT_OBJECT: return "STT_OBJECT";
        case STT_FUNC: return "STT_FUNC";
        case STT_SECTION: return "STT_SECTION";
        case STT_FILE: return "STT_FILE";
        case STT_HIPROC: return "STT_HIPROC";
        default:  return "STT_UNKNOWN";
    }
}

char *symBind(elf_t *elf, unsigned char info)
{
    switch(elf->cls == ELFCLASS32 ? ELF32_ST_TYPE(info) : ELF64_ST_TYPE(info)) {
        case STB_LOCAL: return "STB_LOCAL";
        case STB_GLOBAL: return "STB_GLOBAL";
        case STB_WEAK: return "STB_WEAK";
        case STB_LOPROC: return "STB_LOPROC";
        case STB_HIPROC: return "STB_HIPROC";
        default:  return "STB_UNKNOWN";
    }
}

char *getSymName(elf_t *elf, int idx, uint32_t name)
{
    int strshndx = SHDR_M(elf, elf->symtab_shndx, sh_link);

    if (idx >= SYMNUM(elf) || name == 0) 
        return "null";
    
    return getStr(elf, strshndx, name);
}

void outputSym(elf_t *elf, int idx)
{
    uint32_t name = SYM_M(elf, idx, st_name);
    unsigned char info = SYM_M(elf, idx, st_info);
    printf(" [%03d]    %08x    ", idx, name);
    printf("%-*s", SYMTYPELEN, symType(elf, info));
    printf("%-*s", SYMBINDLEN, symBind(elf, info));
    
    printf("%s", getSymName(elf, idx, name));

    printf("\n");
}

void outputSymtab(elf_t *elf)
{
    int i;

    if (!elf || elf->symtab_shndx == 0) {
        log_e("elf invalid\n");
        return;
    }

    outputSymTitle();
    
    for (i = 0; i < SYMNUM(elf); i++) {
        outputSym(elf, i);
    }

}