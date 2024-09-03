#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <elf.h>

#include "parse_file.h"
#include "header.h"
#include "global.h"

static elf_t *elf_create(const char *filename, const char *file_map, size_t file_size)
{
    elf_t *elf;

    elf = (elf_t *)malloc(sizeof(elf_t));
    if (!elf) {
        fprintf(stderr, "Memory lack\n");
        return NULL;
    }

    memset(elf, 0, sizeof(elf_t));
    elf->filename = filename;
    elf->map = file_map;
    elf->size = file_size;

    // 设置Ehdr
    if (setEhdr(elf) < 0) {
        free(elf);
        return NULL;
    }


}


int parse_file(const char *file)
{
    int fd;
    struct stat stbuf;
    unsigned char *file_map;
    size_t file_size;
    int class, ret = -1;

    if ((fd = open(file, O_RDONLY)) < 0) {
        fprintf(stderr, "open file %s: %s\n", file, strerror(errno));
        return -1;
    }

    memset(&stbuf, 0, sizeof(stbuf));
    if (fstat(fd, &stbuf) < 0) {
        fprintf(stderr, "stat file %s: %s\n", file, strerror(errno));
        return -1;
    }

    file_size = stbuf.st_size;
    file_map = (unsigned char *)mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (file_map == MAP_FAILED) {
        fprintf(stderr, "mmap file %s: %s\n", file, strerror(errno));
        return -1;
    }
    close(fd);

    elf_t *elf = elf_create(file, file_map, file_size);
    if (!elf)
        return -1;
    
    if (elf_head == NULL) 
        elf_head =  elf;
    else 
        *elf_tail = elf;
    elf_tail = &elf->next;

out:
    munmap(file_map, stbuf.st_size);
    return ret;
}
