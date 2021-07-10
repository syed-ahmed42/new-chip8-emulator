#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "instructions.h"
#include "chip8.h"

void draw(struct chip_t *c8);
int init_SDL();
void clean_up();
int is_close_requested();

int main(void)
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
