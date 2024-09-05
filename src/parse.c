#include "parse.h"
#include "logger.h"
#include "global.h"
#include "ehdr.h"
#include "section.h"
#include "symtab.h"
#include "dynsym.h"

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

static void elf_destroy(elf_t *elf)
{
    if (!elf)   return;
    if (elf->map && elf->size > 0) {
        munmap((void*)elf->map, elf->size);
    }

    if (elf->shdr) {
        free(elf->shdr);
    }
    elf->shdr = NULL;

    if (elf->sym)
        free(elf->sym);
    elf->sym = NULL;

    if (elf->dynsym)
        free(elf->dynsym);
    elf->dynsym = NULL;

    if (elf->prg) 
        free(elf->prg);
    elf->prg = NULL;

    free(elf);
}

elf_t *elf_create(char *filename, char *map, size_t size)
{
    elf_t *elf;

    elf = (elf_t *)malloc(sizeof(elf_t));
    if (!elf) {
        log_er(NULL, "Memory lack\n");
    }
    memset(elf, 0, sizeof(elf_t));
    elf->map = map;
    elf->size = size;
    elf->filename = filename;
    elf->destroy = elf_destroy;

    if (setEhdr(elf) < 0) {
        elf_destroy(elf);
        return NULL;
    }

    if (setShdr(elf) < 0) {
        elf_destroy(elf);
        return NULL;
    }

    if (setSym(elf) < 0) {
        log_e("set symtab error\n");
    }

    if (setDynsym(elf) < 0) {
        log_e("set dynsym error\n");
    }

    return elf;
}



int parse_file(char *filename)
{
    int fd;
    struct stat stbuf;
    char *file_map;
    size_t file_size;
    elf_t *elf;

    if ((fd = open(filename, O_RDONLY)) < 0) {
        log_er(-1, "open file %s: %s\n", filename, strerror(errno));
    }

    memset(&stbuf, 0, sizeof(stbuf));
    if (fstat(fd, &stbuf) < 0) {
        log_er(-1,"stat file %s: %s\n", filename, strerror(errno));
    }

    file_size = stbuf.st_size;
    file_map = (char *)mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (file_map == MAP_FAILED) {
        log_er(-1, "mmap file %s: %s\n", filename, strerror(errno));
    }
    close(fd);

    elf = elf_create(filename, file_map, file_size);
    if (!elf) 
        return -1;

    if (elf_head == NULL)
        elf_head =  elf;
    else
        *elf_tail = elf;
    elf_tail = &elf->next;

}