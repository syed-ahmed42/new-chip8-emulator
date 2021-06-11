#include <stdlib.h>
#include <stdint.h>
#include "instructions.h"
#include "chip8.h"

void draw(struct chip_t *c8, uint32_t *buffer);
int init_SDL();
void clean_up();
int is_close_requested();

int main(void)
{
    uint32_t *buffer = malloc((640 * 480) * sizeof(uint32_t));
    struct chip_t c8;

    init_SDL();
    while(!is_close_requested())
    {
        draw(&c8, buffer);
    }
    //cls(&c8);
    clean_up();
    free(buffer);
    return 0;
}
