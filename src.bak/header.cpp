#include "header.h"
#include "logger.h"
#include "global.h"

#include <stdio.h>
#include <elf.h>

int check_ident(unsigned char *ident, int *class)
{
    if (ident[EI_MAG0] != ELFMAG0 || 
        ident[EI_MAG1] != ELFMAG1 ||
        ident[EI_MAG2] != ELFMAG2 ||
        ident[EI_MAG3] != ELFMAG3) {
        fprintf(stderr, "EI_MAG error\n");
        return -1;
    }
    
    *class = 0;
    switch (ident[EI_CLASS])
    {
    case ELFCLASSNONE:
        fprintf(stderr, "EI_CLASS is invalid\n");
        return -1;
    case ELFCLASS32:
    case ELFCLASS64:
        *class = ident[EI_CLASS];
        break;
    default:
        fprintf(stderr, "EI_CLASS unsupported\n");
        return -1;
    }

    if (ident[EI_DATA] != ELFDATA2LSB && ident[EI_DATA] != ELFDATA2MSB) {
        fprintf(stderr, "EI_DATA unsupported\n");
        return -1;
    }

    if (ident[EI_VERSION] != EV_CURRENT) {
        fprintf(stderr, "EI_VERSION is invalid");
        return -1;
    }

    return 0;
}

int check_header_64(Elf64_Ehdr *ehdr)
{
    if (ehdr->e_type == ET_NONE) {
        fprintf(stderr, "Unknown type\n");
        return -1;
    }

    if (ehdr->e_machine == EM_NONE) {
        fprintf(stderr, "Unknown machine\n");
        return -1;
    }

    if (ehdr->e_version == EV_NONE) {
        fprintf(stderr, "Invalid version\n");
        return -1;
    }

    return 0;
}

static const char *type_str(uint16_t type)
{
    switch (type)
    {
    case ET_REL:
        return "REL (Relocatable file)";
    case ET_EXEC:
        return "EXEC (Executable file)";
    case ET_DYN:
        return "DYN (Shared file)";
    case ET_CORE:
        return "CORE (Core file)";
    default:
        return "Unknown type file";
        break;
    }
}

void output_header(elf_t *elf)
{
    int i, width = 33;  

    printf("ELF Header:\n");
    
    printf("  Magic:  ");
    for (i = 0; i < EI_NIDENT; i++) 
        printf("%02x ", EHDR(elf)->e_ident[i]);
    printf("\n");

    printf("  %-*s %s\n", width, "Class:", "ELF64");
    printf("  %-*s 2's complement, %s endian\n", width, "Data:", EHDR(elf)->e_ident[EI_DATA] == ELFDATA2LSB ? "little" : "big");
    printf("  %-*s %d(current)\n", width, "Version:", EV_CURRENT);
    printf("  %-*s %s\n", width, "OS/ABI:", EHDR(elf)->e_ident[EI_OSABI] == ELFOSABI_SYSV ? "UNIX - System V" : "Unknown system");
    printf("  %-*s %d\n", width, "ABI Version:", EHDR(elf)->e_ident[EI_ABIVERSION]);
    printf("  %-*s %s\n", width, "Type:", type_str(EHDR(elf)->e_type));
    printf("  %-*s %d\n", width, "Machine:", EHDR(elf)->e_machine);
    printf("  %-*s 0x%0x\n", width, "Version:", EHDR(elf)->e_version);
    printf("  %-*s 0x%0x\n", width, "Entry point address:", EHDR(elf)->e_entry);
    printf("  %-*s %d (bytes into file)\n", width, "Start of program headers:", EHDR(elf)->e_phoff);
    printf("  %-*s %d (bytes into file)\n", width, "Start of section headers:", EHDR(elf)->e_shoff);
    printf("  %-*s 0x%0x\n", width, "Flags:", EHDR(elf)->e_flags);
    printf("  %-*s %d (bytes)\n", width, "Size of this header:", EHDR(elf)->e_ehsize);
    printf("  %-*s %d (bytes)\n", width, "Size of program headers:", EHDR(elf)->e_phentsize);
    printf("  %-*s %d\n", width, "Number of program headers:", EHDR(elf)->e_phnum);
    printf("  %-*s %d (bytes)\n", width, "Size of section headers:", EHDR(elf)->e_shentsize);
    printf("  %-*s %d\n", width, "Number of section headers:", EHDR(elf)->e_shnum);
    printf("  %-*s %d\n", width, "Section header string table index:", ((Elf64_Ehdr *)(elf->Elf_Ehdr))->e_shstrndx);
}

int setEhdr(elf_t *elf)
{
    char *p;

    if (!elf || !elf->filename || !elf->map || elf->size < EI_NIDENT) {
        log_er(-1, "elf is invalid\n");
    }

    p = elf->map;
    // 检查前4个字节
    if (p[EI_MAG0] != ELFMAG0 || p[EI_MAG1] != ELFMAG1 || p[EI_MAG2] != ELFMAG2 || p[EI_MAG3] != ELFMAG3) {
        log_er(-1, "file %s is not a ELF file\n", elf->filename);
    }

    elf->cls = p[EI_CLASS];
    if (elf->cls != ELFCLASS32 || elf->cls != ELFCLASS64) {
        log_er(-1, "file class %d is invalid\n", elf->cls);
    }

    elf->Ehdr.ehdr = (void*)elf->map;
    // if (EHDR_SIZE(elf) > elf->size || (elf->class == ELFCLASS32 ? ((Elf32_Ehdr *)(elf->Elf_Ehdr)) : ((Elf64_Ehdr *)(elf->Elf_Ehdr)))->e_phoff > elf->size || ((Elf64_Ehdr*)(elf->Elf_Ehdr))->e_shoff > elf->size) {
    //     log_er(-1, "file size %ld is invalid\n", elf->size);
    // }

    (elf->class == ELFCLASS32 ? elf->Ehdr.ehdr32 : elf->Ehdr.ehdr64)->e_ehsize = 100;

    if (elf->class == ELFCLASS32)
        elf->Ehdr.ehdr32->e_ehsize = 50;
    else
        elf->Ehdr.ehdr64->e_ehsize = 100;

    
    return 0;
}