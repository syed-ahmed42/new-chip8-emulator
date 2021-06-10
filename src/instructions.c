#include "chip8.h"
#include <stdio.h>

void cls(struct chip_t *c8)
{
    for (int i = 0; 
         i < SCREEN_WIDTH * SCREEN_HEIGHT;
         i++)
    {
        c8->screen[i] = 0;       
    }

}
