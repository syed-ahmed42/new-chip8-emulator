#ifndef LIBSDL_H
#define LIBSDL_H

#include "chip8.h"
#include "SDL2/SDL.h"

void draw(struct chip_t *c8);
int init_SDL();
void clean_up();
int is_close_requested();

#endif