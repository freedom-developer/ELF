#include "section.h"
#include "utils.h"
#include "global.h"
#include <string.h>
#include <stdlib.h>

static unsigned char *file_start;
static size_t file_len;

static Elf64_Ehdr *ehdr;
static Elf64_Shdr *first_shdr, *shstrtab, *symtab, *strtab, *dynsym, *dynstr, *dynamic;

static Elf64_Half shnum;
static Elf64_Sym **sym_tab, **sym_dyn;
int sym_tab_nr, sym_dyn_nr;
int global_var = 100;

static const char *stype_str[] = {
    _string(SHT_NULL), _string(SHT_PROGBITS), _string(SHT_SYMTAB), _string(SHT_STRTAB),
    _string(SHT_RELA), _string(SHT_HASH), _string(SHT_DYNAMIC), _string(SHT_NOTE),
    _string(SHT_NOBITS), _string(SHT_REL), _string(SHT_SHLIB), _string(SHT_DYNSYM),
};

static const char *sym_vis[] = {
    _string(STV_DEFAULT), _string(STV_INTERNAL), _string(STV_HIDDEN), _string(STV_PROTECTED)
};

static const char *get_string(Elf64_Shdr *shdr, int name)
{
    if (shdr->sh_type != SHT_STRTAB) {
        fprintf(stderr, "get_string: the type of shdr is not a STRTAB\n");
        return NULL;
    }

    if (shdr->sh_offset + shdr->sh_size > file_len || name > shdr->sh_size) {
        fprintf(stderr, "the end pos of this section is exceed file length\n");
        return NULL;
    }

    return (const char *)(file_start + shdr->sh_offset + name);
}

static const char *section_type(Elf64_Word type)
{
    static char stype[100];

    if (type >= ARRAY_SIZE(stype_str)) {
        memset(stype, 0, 100);
        snprintf(stype, 100, "Unknown section type: 0x%0x", type);
        return stype;
    }
    return stype_str[type];
}

static const char *section_flags(Elf64_Word flags)
{
    static char sflags[100];

    memset(sflags, 0, 100);
    snprintf(sflags, 100, "(0x%0x) ", flags);
    if (flags == SHF_MASKPROC)  return _string(SHF_MASKPROC);
    if (flags & SHF_ALLOC) 
        snprintf(sflags + strlen(sflags), 100 - strlen(sflags), _string(SHF_ALLOC));
    if (flags & SHF_WRITE)
        snprintf(sflags + strlen(sflags), 100 - strlen(sflags), " | " _string(SHF_WRITE));
    if (flags & SHF_EXECINSTR)
        snprintf(sflags + strlen(sflags), 100 - strlen(sflags), " | " _string(SHF_EXECINSTR));
    return sflags;
}

static void print_section_header_64(Elf64_Shdr *shdr)
{
    printf("  name: %s\n", get_string(shstrtab, shdr->sh_name));
    printf("  type: %s\n", section_type(shdr->sh_type));
    printf("  flags: %s\n", section_flags(shdr->sh_flags));
    printf("  address: 0x%0x\n", shdr->sh_addr);
    printf("  offset: 0x%0x\n", shdr->sh_offset);
    printf("  size: 0x%0x\n", shdr->sh_size);
    printf("  link: %d\n", shdr->sh_link);
    printf("  info: %d\n", shdr->sh_info);
    printf("  addralign: %d\n", shdr->sh_addralign);
    printf("  entsize: %d\n", shdr->sh_entsize);
}

static void print_shdr_all_64(void)
{
    int i;
    for (i = 0; i < shnum; i++) {
        printf("section %d\n", i);
        print_section_header_64(first_shdr + i);
    }
}

static void print_strtab(Elf64_Shdr *shdr)
{
    char *s, *e, *p;

    if (!shdr || shdr->sh_type != SHT_STRTAB) {
        fprintf(stderr, "print_strtab: shdr is not a STRTAB section\n");
        return;
    }

    printf("section %s:\n", get_string(shstrtab, shdr->sh_name));
    p = s = file_start + shdr->sh_offset;
    e = s + shdr->sh_size;
    while (p < e) {
        if (!*p) { 
            p++;
            continue;
        }
        printf("\t%s\n", p);
        p += strlen(p);
    }
}


// 打印符号表中的一个符号
static void print_sym(Elf64_Sym *sym, Elf64_Shdr *_strtab)
{
    int has_value = 0;
    if (sym->st_name == 0) 
        printf("name: this sym have no name\n");
    else
        printf("name: %s\n", get_string(_strtab, sym->st_name));

    printf("\tbind: ");
    switch (ELF64_ST_BIND(sym->st_info))
    {
    case STB_LOCAL: printf("STB_LOCAL\n"); break;
    case STB_GLOBAL: printf("STB_GLOBAL\n"); break;
    case STB_WEAK: printf("STB_WEAK\n"); break;
    case STB_LOPROC: printf("STB_LOPROC\n"); break;
    case STB_HIPROC: printf("STB_HIPROC\n"); break;
    default:
        printf("Unknown visiable attribute: %d\n", ELF64_ST_BIND(sym->st_info));
        break;
    }

    printf("\ttype: ");
    switch (ELF64_ST_TYPE(sym->st_info)) 
    {
    case STT_NOTYPE: printf(_string(STT_NOTYPE) "\n"); break;
    case STT_OBJECT: has_value = 1; printf(_string(STT_OBJECT) "\n"); break;
    case STT_FUNC: has_value = 1; printf(_string(STT_FUNC) "\n"); break;
    case STT_SECTION: printf(_string(STT_SECTION) "\n"); break;
    case STT_FILE: printf(_string(STT_FILE) "\n"); break;
    case STT_LOPROC: printf(_string(STT_LOPROC) "\n"); break;
    case STT_HIPROC: printf(_string(STT_HIPROC) "\n"); break;
    default:
        printf("Unknown type: %d\n", ELF64_ST_TYPE(sym->st_info));
        break;
    }
    
    printf("\tvisiuable: %s\n", sym->st_other > ARRAY_SIZE(sym_vis) ? "Unknown vis" : sym_vis[sym->st_other]);

    printf("\tshndx: ");
    if (sym->st_shndx == SHN_ABS) printf("SHN_ABS\n");
    else if (sym->st_shndx == SHN_COMMON) printf("SHN_COMMON\n");
    else if (sym->st_shndx == SHN_UNDEF) printf("SHN_UNDEF\n");
    else {
        printf("%d\n", sym->st_shndx);
    }
    printf("\tvalue: %d\n", sym->st_value);
    printf("\tsize: %d\n", sym->st_size);

    // if (has_value) {
    //     if (sym->st_shndx > 0 && ehdr->e_type == ET_REL) {
    //         printf("\tvalue:\n");
    //         if (sym->st_shndx >= shnum) {
    //             printf("\t\tsym's shndx invalid\n");
    //         } else if (sym->st_size == 0) {
    //             printf("\t\tsym's size invalid\n");
    //         } else {
    //             Elf64_Shdr *shdr = first_shdr + sym->st_shndx;
    //             Elf64_Addr offset = shdr->sh_offset + sym->st_value;
    //             unsigned char *start = file_start + offset;
    //             int size = sym->st_size;
    //             int i;
    //             printf("\t\t0x%016x: ", offset);
    //             for (i = 0; i < size; i++) {
    //                 if (i > 0 && i % 16 == 0) printf("\n\t\t0x%016x: ", offset + i);
    //                 printf("%02x ", *(start + i));
    //             }
    //             printf("\n");
    //         }
    //     }
    // }
}

// 保存所有符号
static void save_syms(Elf64_Shdr *symtab_shdr, Elf64_Sym ***list, int *sym_num)
{
    int i;
    unsigned char *sym_start;

    if (!symtab_shdr) {
        fprintf(stderr, "symtab_shdr is null\n");
        return;
    }
    
    if (file_len < symtab_shdr->sh_offset + symtab_shdr->sh_size) {
        fprintf(stderr, "file length is not enough\n");
        return;
    }
    if (symtab_shdr->sh_entsize <= 0) {
        fprintf(stderr, "sym's entsize is invalid\n");
        return;
    }

    sym_start = file_start + symtab_shdr->sh_offset;
    *sym_num = symtab_shdr->sh_size / symtab_shdr->sh_entsize;
    if (*list)
        free(*list);
    *list = (Elf64_Sym **)calloc(*sym_num, sizeof(Elf64_Sym *));
    if (!*list) {
        fprintf(stderr, "No more memory\n");
        return;
    }

    for (i = 0; i < *sym_num; i++) {
        (*list)[i] = (Elf64_Sym *)(sym_start + (i * symtab_shdr->sh_entsize));
    }
}

static void print_syms(Elf64_Sym **list, Elf64_Shdr *_strtab, int n)
{
    int i;
    
    for (i = 0; i < n; i++) {
        print_sym(list[i], _strtab);
    }
}


void parse_section_64(unsigned char *file_map, size_t file_size)
{
    int i;
    file_start = file_map;
    file_len = file_size;
    ehdr = (Elf64_Ehdr *)file_map;
    shnum = ehdr->e_shnum;
    Elf64_Off shoff = ehdr->e_shoff;
    Elf64_Half shentsize = ehdr->e_shentsize;

    if (file_len < shoff + shnum * shentsize) {
        fprintf(stderr, "file size is not enough\n");
        return;
    }

    first_shdr = (Elf64_Shdr *)(ehdr->e_shoff + file_start);
    if (ehdr->e_shstrndx >= shnum) {
        fprintf(stderr, "shnum error\n");
        return;
    }
    shstrtab = first_shdr + ehdr->e_shstrndx;
    for (i = 0; i < shnum; i++) {
        Elf64_Shdr *tmp = first_shdr + i;
        if (tmp->sh_type == SHT_SYMTAB)
            symtab = tmp;
        else if (tmp->sh_type == SHT_DYNSYM)
            dynsym = tmp;
        else if (tmp->sh_type == SHT_DYNAMIC)
            dynamic = tmp;
        else if (strcmp(get_string(shstrtab, tmp->sh_name), ".strtab") == 0) {
            strtab = tmp;
        } else if (strcmp(get_string(shstrtab, tmp->sh_name), ".dynstr") == 0) {
            dynstr = tmp;
        }
        
        // if (tmp->sh_type == SHT_STRTAB)
        //     print_strtab(tmp);
    }

    // print_strtab(shstrtab);
    // print_strtab(strtab);
    // print_strtab(dynstr);

    save_syms(symtab, &sym_tab, &sym_tab_nr);
    save_syms(dynsym, &sym_dyn, &sym_dyn_nr);

    // print_syms(sym_dyn, strtab, sym_dyn_nr);
    // print_syms(sym_dyn, dynstr, sym_dyn_nr);    
}