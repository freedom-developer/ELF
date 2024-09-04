#include "global.h"
#include "parse_arguments.h"
#include "parse.h"
#include "ehdr.h"
#include "section.h"
#include "symtab.h"


int main(int argc, char **argv)
{
    int i;
    elf_t *elf;

    // 解析参数
    parse_arguments(argc, argv);

    elf_head = NULL;
    elf_tail = NULL;

    // 解析文件
    for (i = 0; optargs.files[i]; i++) {
        parse_file(optargs.files[i]);
    }

    for (elf = elf_head; elf; elf = elf->next) {
        if (optargs.header) { // 输出Ehdr
            outputEhdr(elf);
        }
        if (optargs.sheader) {
            outputAllShdr(elf);
        }

        if (optargs.secs_bits) {
            for (i = 0; i < 64; i++) {
                if (optargs.secs_bits & (1ULL << i)) {
                    outputSector(elf, i);
                }
            }
        }

        if (optargs.symtab) {
            outputSymtab(elf);
        }
    }

    

    


    free_elfs();

    return 0;
}
