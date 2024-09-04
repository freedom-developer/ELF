
#include "strtab.h"
#include "logger.h"
#include <string.h>

char *getStr(elf_t *elf, int shndx, uint32_t name)
{
    if (!elf || !elf->shdr) {
        log_er(NULL, "elf invalid\n");
    }
    if (EHDR_M(elf, e_shnum) < shndx) {
        log_er(NULL, "shndx %d invalid\n", shndx);
    }
    if (SHDR_M(elf, shndx, sh_type) != SHT_STRTAB) {
        log_er(NULL, "The %dth section is not a strtab\n", shndx);
    }
    if (SHDR_M(elf, shndx, sh_offset) + name > elf->size) {
        log_er(NULL, "elf size %ld invalid\n", elf->size);
    }

    return elf->map + SHDR_M(elf, shndx, sh_offset) + name;
}

char *getSecName(elf_t *elf, uint32_t name)
{
    return getStr(elf, EHDR_M(elf, e_shstrndx), name);
}

void outputStrtab(elf_t *elf, int shndx)
{
    if (!elf || !elf->shdr) {
        log_e("elf invalid\n");
        return;
    }
    if (EHDR_M(elf, e_shnum) < shndx || SHDR_M(elf, shndx, sh_offset) + SHDR_M(elf, shndx, sh_size) > elf->size) {
        log_e("shndx %d invalid\n", shndx);
        return;
    }
    if (SHDR_M(elf, shndx, sh_type) != SHT_STRTAB) {
        log_e("The %dth section is not a strtab\n", shndx);
    }

    char *s = elf->map + SHDR_M(elf, shndx, sh_offset);
    char *e = s + SHDR_M(elf, shndx, sh_size);
    int i = 0;
    while (s < e) {
        printf("%02d: %s\n", i++, s);
        s += strlen(s) + 1;
    }
}