#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <mach-o/loader.h>

#define ELF_HEADER_SIZE 0x1000

int main(int argc, char **argv) {
    char *filename = NULL;
    int c;
    
    while ((c = getopt(argc, argv, "f:")) != -1) {
        switch (c) {
            case 'f': filename = optarg; break;
        }
    }

    if (!filename) {
        puts("error: no input files");
        return 1;
    }

    FILE *f = fopen(filename, "r");
    assert(f != NULL);
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *macho_buf = malloc(fsize);
    fread(macho_buf, 1, fsize, f);
    fclose(f);

    char *obj = macho_buf + ELF_HEADER_SIZE;

    struct mach_header_64 *header = (struct mach_header_64 *)obj;
    assert(header->magic == MH_MAGIC_64);

    obj += sizeof(*header);

    struct load_command *lc = (struct load_command *)obj;
    assert(lc->cmd == LC_SEGMENT_64);
    struct segment_command_64 *segment = (struct segment_command_64 *)lc;

    printf("0x%llx\n", segment->vmaddr);

    free(macho_buf);
    return 0;
}
