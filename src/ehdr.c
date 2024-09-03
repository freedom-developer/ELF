#include "ehdr.h"
#include "logger.h"

/*
           typedef struct {
               unsigned char e_ident[EI_NIDENT];
               uint16_t      e_type;
               uint16_t      e_machine;
               uint32_t      e_version;
               ElfN_Addr     e_entry;
               ElfN_Off      e_phoff;
               ElfN_Off      e_shoff;
               uint32_t      e_flags;
               uint16_t      e_ehsize;
               uint16_t      e_phentsize;
               uint16_t      e_phnum;
               uint16_t      e_shentsize;
               uint16_t      e_shnum;
               uint16_t      e_shstrndx;
           } ElfN_Ehdr;
*/

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

    elf->Ehdr.ehdr = elf->map;

    if (elf->size < EHDR_SIZE(elf)) {
        log_er(-1, "file size %ld is invalid\n", elf->size);
    }

    size_t shoff = EHDR_M(elf, e_shoff);

    return 0;
}

void outputEhdr(elf_t *elf)
{
    int i, width = 33;

    printf("ELF Header:\n");

    printf("  Magic:  ");
    for (i = 0; i < EI_NIDENT; i++)
        printf("%02x ", EHDR(elf)->e_ident[i]);
    printf("\n");

    printf("  %-*s %s\n", width, "Class:", "%s", elf->cls == ELFCLASS32 ? "ELF32" : "ELF64");
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