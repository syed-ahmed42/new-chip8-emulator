#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "chip8.h"

#define OPCODE_NNN(opcode) (opcode & 0xFFF)
#define OPCODE_KK(opcode) (opcode & 0xFF)
#define OPCODE_N(opcode) (opcode & 0xF)
#define OPCODE_X(opcode) ((opcode >> 8) & 0xF)
#define OPCODE_Y(opcode) ((opcode >> 4) & 0xF)

#define PIXEL_OFF 0
#define PIXEL_ON 1

static int is_debug = 0;

static void log(const char *msg)
{
    if (is_debug)
    {
        printf("MESSAGE: %s\n", msg);
    }
}

static void set_debug_mode(int debug_mode)
{
    is_debug = debug_mode;
}

typedef void (*opcode_table_t)(struct chip_t *cpu, word opcode);

static int global_delta;

static int hexcodes[] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

static void nibble_0(struct chip_t *cpu, word opcode)
{
    if (opcode == 0x00E0)
    {
        memset(cpu->screen, 0,
         SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(uint32_t));
    }
    else if (opcode == 0x00EE)
    {
       if (cpu->pc > 0) 
       {
           cpu->pc = cpu->stack[cpu->sp];
           cpu->sp--;
       }
    }
}

static void nibble_1(struct chip_t *cpu, word opcode)
{
   cpu->pc = OPCODE_NNN(opcode);
}

static void nibble_2(struct chip_t *cpu, word opcode)
{
    if (cpu->sp < 16)
    {
        cpu->sp++;
        cpu->stack[cpu->sp] = cpu->pc;
        cpu->pc = OPCODE_NNN(opcode);
    }
}

static void nibble_3(struct chip_t *cpu, word opcode)
{
    if (cpu->v[OPCODE_X(opcode)]== OPCODE_KK(opcode))
    {
        cpu->pc = (cpu->pc + 2) & 0xFFF;
    }
}

static void nibble_4(struct chip_t *cpu, word opcode)
{
    if (cpu->v[OPCODE_X(opcode)] != OPCODE_KK(opcode))
    {
        cpu->pc = (cpu->pc + 2) & 0xFFF;
    }
}

static void nibble_5(struct chip_t *cpu, word opcode)
{
   if (cpu->v[OPCODE_X(opcode)] == cpu->v[OPCODE_Y(opcode)]) 
   {
        cpu->pc = (cpu->pc + 2) & 0xFFF;
   }
}

static void nibble_6(struct chip_t *cpu, word opcode)
{
    cpu->v[OPCODE_X(opcode)] = OPCODE_KK(opcode);
}

static void nibble_7(struct chip_t *cpu, word opcode)
{
    cpu->v[OPCODE_X(opcode)] += OPCODE_KK(opcode);
}

static void nibble_8(struct chip_t *cpu, word opcode)
{
    byte x = OPCODE_X(opcode), y = OPCODE_Y(opcode);
    switch(OPCODE_N(opcode))
    {
        case 0:
           cpu->v[x] = cpu->v[y];
           break;
        case 1:
            cpu->v[x] |= cpu->v[y];
            break;
        case 2:
            cpu->v[x] &= cpu->v[y];
            break;
        case 3:
            cpu->v[x] ^= cpu->v[y];
            break;
        case 4:
            cpu->v[0xF] = cpu->v[x] > ((cpu->v[x] + cpu->v[y]) & 0xFF);
            cpu->v[x] += cpu->v[y];
            break;
        case 5:
            cpu->v[0xF] = (cpu->v[x] > cpu->v[y]);
            cpu->v[x] -= cpu->v[y];
            break;
        case 6:
            cpu->v[0xF] = (cpu->v[x] & 1);
            cpu->v[x] >>= 1;
            break;
        case 7:
            cpu->v[0xF] = (cpu->v[y] > cpu->v[x]);
            cpu->v[x] = cpu->v[y] - cpu->v[x];
            break;
        case 0xE:
            cpu->v[0xF] = ((cpu->v[x] & 0x80) != 0);
            cpu->v[x] *= 2;
            break;
    }
}

static void nibble_9(struct chip_t *cpu, word opcode)
{
    if (cpu->v[OPCODE_X(opcode)] != cpu->v[OPCODE_Y(opcode)])
    {
        cpu->pc = (cpu->pc + 2) & 0xFFF;
    }
}

static void nibble_A(struct chip_t *cpu, word opcode)
{
    cpu->i = OPCODE_NNN(opcode);
}

static void nibble_B(struct chip_t *cpu, word opcode)
{
    cpu->pc = (OPCODE_NNN(opcode) + cpu->v[0]) & 0xFFF;
}

static void nibble_C(struct chip_t *cpu, word opcode)
{
    cpu->v[OPCODE_X(opcode)] = rand() & OPCODE_KK(opcode);
}

static void nibble_D(struct chip_t *cpu, word opcode)
{
    byte x = OPCODE_X(opcode), y = OPCODE_Y(opcode);
    cpu->v[0xF] = 0;

    for (int m = 0; m < OPCODE_N(opcode); m++)
    {
        byte sprite = cpu->memory[cpu->i + m];
        int32_t row = (cpu->v[y] + m) % 480;

        for (int p = 0; p < 8; p++)
        {
            int32_t b = (sprite & 0x80) >> 7;
            int32_t col = (cpu->v[x] + p) % 640;
            int32_t offset = row * 640 + col;

            if (b == 1)
            {
                if (cpu->screen[offset] != PIXEL_OFF)
                {
                    cpu->screen[offset] = PIXEL_OFF;
                    cpu->v[0xF] = 1;
                }
                else
                {
                    cpu->screen[offset] = PIXEL_ON;
                }

                sprite <<= 1;
            }
        }
    }
}

static void nibble_E(struct chip_t *cpu, word opcode)
{
    char key = cpu->v[OPCODE_X(opcode)];

    if (OPCODE_KK(opcode) == 0x9E)
    {
        if (cpu->keydown && cpu->keydown(key & 0xF))
        {
            cpu->pc = (cpu->pc + 2) & 0xFFF;
        }
    }
    else if (OPCODE_KK(opcode) == 0xA1)
    {
        if (cpu->keydown && !cpu->keydown(key & 0xF))
        {
            cpu->pc = (cpu->pc + 2) & 0xFFF;
        }
    }
}

static void nibble_F(struct chip_t *cpu, word opcode)
{
    byte x = OPCODE_X(opcode);

    switch(OPCODE_KK(opcode))
    {
        case 0x07:
            cpu->v[x] = cpu->delay_timer;
            break;
        case 0x0A:
            cpu->wait_key = OPCODE_X(opcode);
            break;
        case 0x15:
            cpu->delay_timer = cpu->v[x];
            break;
        case 0x18:
            cpu->sound_timer = cpu->v[x];
            break;
        case 0x1E:
            cpu->i += cpu->v[x];
            break;
        case 0x29:
            cpu->i = 0x50 + (cpu->v[x] & 0xF) * 5;
            break;
        case 0x33:
            cpu->memory[cpu->i + 2] = cpu->v[x] % 10;
            cpu->memory[cpu->i + 1] = (cpu->v[x] / 10) % 10;
            cpu->memory[cpu->i] = cpu->v[x] / 100;
            break;
        case 0x55:
            for (int k = 0; k <= x; k++)
            {
                cpu->memory[cpu->i + k] = cpu->v[k];
            }
            break;
        case 0x65:
            for (int k = 0; k <= x; k++)
            {
                cpu->v[k] = cpu->memory[cpu->i + k];
            }
            break;
    }
}
static opcode_table_t nibbles[16] = {
    &nibble_0, &nibble_1, &nibble_2, &nibble_3,
    &nibble_4, &nibble_5, &nibble_6, &nibble_7,
    &nibble_8, &nibble_9, &nibble_A, &nibble_B,
    &nibble_C, &nibble_D, &nibble_E, &nibble_F 
};

void init(struct chip_t *cpu)
{
    memset(cpu, 0x00, sizeof(struct chip_t));
    memcpy(cpu->memory + 0x50, hexcodes, 80);
    cpu->pc = 0x200;
    cpu->wait_key = -1;
    global_delta = 0;
    log("Debug mode is enabled");
    log("Machine has been initialized");
}

void update_time(struct chip_t *cpu, int delta)
{
    global_delta += delta;
    while (global_delta > (1000 / 60)) {
        global_delta -= (1000 / 60);
        if (cpu->delay_timer > 0) {
            cpu->delay_timer--;
        }
        if (cpu->sound_timer > 0) {
            if (--cpu->sound_timer == 0 && cpu->speaker) {
                /* Disable speaker buzz. */
                cpu->speaker(0);
            } else if (cpu->speaker) {
                /* Enable speaker buzz. */
                cpu->speaker(1);
            }
        }
    }
}