#ifndef _SECTION_H
#define _SECTION_H

#include "global.h"

int setShdr(elf_t *elf);

void outputAllShdr(elf_t *elf);

void outputSector(elf_t *elf, int shndx);

#endif