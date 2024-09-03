#define DEFINE_VAR
#include "global.h"

void free_elfs(void)
{
    elf_t *elf, **next;

    for (elf = elf_head; elf; next = &elf->next, elf = *next) {
        elf->destroy(elf);
    }
}