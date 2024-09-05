#ifndef _DYNSYM_H
#define _DYNSYM_H

#include "global.h"

int setDynsym(elf_t *elf);

void outputDynsymtab(elf_t *elf);

#endif