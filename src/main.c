#include <stdlib.h>
#include <stdint.h>
#include "instructions.h"
#include "chip8.h"

void start();

int main(void)
{
    struct chip_t c8;

    start(&c8);
    cls(&c8);
    return 0;
}
