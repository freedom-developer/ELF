#include "program.h"
#include "utils.h"

#include <stdlib.h>
#include <string.h>

static Elf64_Ehdr *ehdr;
phdr_array_t phdr_array;
static unsigned char *file_start;
static size_t file_size;

static const char *type_string(int type)
{
    switch (type)
    {
    case PT_NULL: return "NULLL";
    case PT_LOAD: return "LOAD";
    case PT_DYNAMIC: return "DYNAMIC";
    case PT_INTERP: return "INTERP";
    case PT_NOTE: return "NOTE";
    case PT_SHLIB: return "SHLIB";
    case PT_PHDR: return "PHDR";
    case PT_TLS:  return "TLS";
    case PT_NUM: return "NUM";
    case PT_GNU_EH_FRAME: return "GNU_EH_FRAME";
    case PT_GNU_STACK: return "GNU_STACK";
    case PT_GNU_RELRO: return "GNU_RELRO";
    case PT_LOPROC: return "LOPROC";
    case PT_HIPROC: return "HIPROC";
    default:
        return "Unknown type";
        break;
    }
}

static void print_phdr(Elf64_Phdr *phdr)
{
    printf("%10s\t%016x\t%016x\t%016x\t%016x\t%016x\t%10d\t%10d\n", 
        type_string(phdr->p_type), 
        phdr->p_offset,
        phdr->p_vaddr,
        phdr->p_paddr,
        phdr->p_filesz,
        phdr->p_memsz,
        phdr->p_flags,
        phdr->p_align
        );  
}

static void print_interp(Elf64_Phdr *interp_phdr)
{
    if (!interp_phdr || interp_phdr->p_type != PT_INTERP) {
        fprintf(stderr, "invalid argument\n");
        return;
    }
    printf("interp: %s\n", (char *)(file_start + interp_phdr->p_offset));
}


void parse_program(unsigned char *elf_map, size_t elf_size)
{
    int i;
    Elf64_Phdr *first_phdr;

    ehdr = (Elf64_Ehdr *)elf_map;
    file_start = elf_map;
    file_size = elf_size;

    if (file_size < sizeof(Elf64_Ehdr)) {
        fprintf(stderr, "file size is not enought\n");
        return;
    }

    if (file_size < ehdr->e_phoff + ehdr->e_phnum * ehdr->e_phentsize) {
       fprintf(stderr, "file size is not enought\n");
        return; 
    }

    if (ehdr->e_phentsize != sizeof(Elf64_Phdr)) {
        fprintf(stderr, "ehdr->e_phentsize invlid\n");
        return;
    }

    phdr_array.phdr_array = (Elf64_Phdr **)calloc(ehdr->e_phnum, sizeof(Elf64_Phdr *));
    if (!phdr_array.phdr_array) {
        fprintf(stderr, "No more memory\n");
        return;
    }

    phdr_array.phnum = ehdr->e_phnum;
    first_phdr = (Elf64_Phdr *)(file_start + ehdr->e_phoff);
    for (i = 0; i < ehdr->e_phnum; i++) {
        phdr_array.phdr_array[i] = first_phdr + i;
    }

    for (i = 0; i < ehdr->e_phnum; i++) {
        Elf64_Phdr * phdr = phdr_array.phdr_array[i];
        if (phdr->p_type == PT_INTERP)
            print_interp(phdr);
    }

}