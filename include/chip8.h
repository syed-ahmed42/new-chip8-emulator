#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>

#define MEMSIZE 4096
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

typedef uint8_t byte;
typedef uint16_t word;
typedef uint16_t address;


typedef int (*keyboard_poller_t)(char);
typedef void (*speaker_handler_t)(int);

struct chip_t{
    address memory[MEMSIZE];
    address pc;

    byte v[16];
    address i;

    address stack[16];
    byte sp;

    byte sound_timer;
    byte delay_timer;

    uint32_t screen[SCREEN_HEIGHT * SCREEN_WIDTH];

    keyboard_poller_t keydown;
    speaker_handler_t speaker;

    char wait_key;

    int exit;
};

void init(struct chip_t *cpu);
void update_time(struct chip_t *cpu, int delta);

#endif
