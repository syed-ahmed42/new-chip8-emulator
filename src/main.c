#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#include "libsdl.h"
#include "chip8.h"


static int load_rom(struct chip_t *cpu, const char *file)
{
    FILE *rom = fopen(file, "rb");
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

    if (init_SDL()) {
        fprintf(stderr, "Error initializing SDL graphical context:\n");
        fprintf(stderr, "%s\n", SDL_GetError());
        return 1;
    }

    srand(time(NULL));

    init(&c8);
    return 0;
}
