#ifndef _STRTAB_H
#define _STRTAB_H

#include "global.h"

char *getStr(elf_t *elf, int shndx, uint32_t name);
char *getSecName(elf_t *elf, uint32_t name);
void outputStrtab(elf_t *elf, int shndx);

#endif