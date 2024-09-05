#ifndef _SYMTAB_H
#define _SYMTAB_H

#include "global.h"

#define SYMNAMELEN 12
#define SYMTYPELEN 15
#define SYMBINDLEN 20
#define SYMVISLEN 15
#define SYMSHNLEN 8
#define SYMSIZELEN 20
#define SYMVALLEN 20

int setSym(elf_t *elf);

void outputSymtab(elf_t *elf);

void outputSymTitle(void);
char *symType(elf_t *elf, unsigned char info);
char *symBind(elf_t *elf, unsigned char info);
char *symVis(elf_t *elf, unsigned char other);

#endif