#include "chip8.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define OPCODE_NNN(opcode) (opcode & 0xFFF)
#define OPCODE_KK(opcode) (opcode & 0xFF)
#define OPCODE_N(opcode) (opcode & 0xF)
#define OPCODE_X(opcode) ((opcode >> 8) & 0xF)
#define OPCODE_Y(opcode) ((opcode >> 4) & 0xF)

#define PIXEL_OFF 0
#define PIXEL_ON 1

typedef void (*opcode_table_t) (struct chip_t *cpu, word opcode);

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

static opcode_table_t nibbles[16] = {
    &nibble_0, &nibble_1, &nibble_2, &nibble_3,
    &nibble_4, &nibble_5, &nibble_6, &nibble_7,
    &nibble_8, &nibble_9, &nibble_A, &nibble_B,
    &nibble_C, &nibble_D 
};
