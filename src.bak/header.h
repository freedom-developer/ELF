#ifndef _HEADER_H
#define _HEADER_H

#include "global.h"

int check_ident(unsigned char *ident, int *class);
int check_header_64(Elf64_Ehdr *ehdr);
void print_header_64(Elf64_Ehdr *ehdr);

int setEhdr(elf_t *elf);


#endif
