#define DEFINE_VAR
#include "global.h"

uint64_t g_test = 1000;

void free_elfs(void)
{
    elf_t *elf, **next;

    for (elf = elf_head; elf; next = &elf->next, elf = *next) {
        elf->destroy(elf);
    }
}