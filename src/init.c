#include "SDL2/SDL.h"
#include <stdlib.h>
#include <stdint.h>
#include "chip8.h"

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static SDL_Texture *texture = NULL;

int draw(struct chip_t *c8, uint32_t *buffer)
{
    for (int i = 0; i < 640 * 480; i++)
    {
        c8->screen[i] = 0x000000FF;
    }
    //Controlling which part of the screen 
    //is gonna be white
    for (int i = 0; i < 640 * 47; i++)
    {
        c8->screen[i] = 0xFFFFFFFF;
    }
    //Transferring the entire screen
    //to the buffer
    /*SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);*/
    int pitch = 640 * sizeof(uint32_t);
    void *pixels = buffer;
    for (int i = 0; i < 640 * 480; i++)
    {
        buffer[i] = c8->screen[i];
    }

    SDL_LockTexture(texture, 
        NULL, 
        pixels, 
        &pitch
    );
    SDL_UnlockTexture(texture);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    
    /*Updating the screen with the buffer
    SDL_UpdateTexture(texture, NULL, buffer, 640 * sizeof(uint32_t));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);*/

    return 0;
}

int is_close_requested()
{
    int done = 0;
    while (!done)
    {
        SDL_Event event;

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                return 1;
            }
        }
    }
    return 0;
}

int init_SDL()
{
    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow(
        "AN SDL WINDOW",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        640,
        480,
        SDL_WINDOW_OPENGL
    );

    if (window == NULL)
    {
        printf("Failed to create window: %s\n", SDL_GetError());
        return 1;
    }

    renderer = SDL_CreateRenderer(window, -1, 
            SDL_RENDERER_ACCELERATED
            );

    if (renderer == NULL)
    {
        printf("Failed to create renderer: %s\n", SDL_GetError());
        return 1;
    }

    texture = SDL_CreateTexture(
            renderer,
            SDL_PIXELFORMAT_RGBA8888,
            SDL_TEXTUREACCESS_STREAMING,
            640,
            480
    );
    return 0;
}

void clean_up()
{
    if (window != NULL)
    {
        SDL_DestroyWindow(window);
    }
    if (renderer != NULL)
    {
        SDL_DestroyRenderer(renderer);
    }
    SDL_Quit();
}
