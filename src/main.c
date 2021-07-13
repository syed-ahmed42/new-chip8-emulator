#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "instructions.h"
#include "chip8.h"
#include <string.h>


static int load_rom(struct chip_t *cpu, const char *file)
{
    FILE *rom = fopen(file, 'rb');
    if (rom == NULL)
    {
        fprintf(stderr, "ERROR: COULD NOT OPEN ROM.\n");
        return 1;
    }

    fseek(rom, 0, SEEK_END);
    int fsize = ftell(rom);
    fseek(rom, 0, SEEK_SET);

    if (fsize > 3584)
    {
        fprintf(stderr, "ERROR: ROM TOO BIG.\n");
        return 1;
    }

    fread(cpu->memory + 0x200, fsize, 1, rom);
    fclose(rom);

    return 0;
}

int main(int argc, char *argv[])
{
    struct chip_t c8;

    init_SDL();
    while(!is_close_requested())
    {
        draw(&c8);
    }
    //cls(&c8);
    clean_up();
    return 0;
}
