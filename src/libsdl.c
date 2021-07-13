#include <stdlib.h>
#include <stdint.h>
#include "libsdl.h"

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static SDL_Texture *texture = NULL;
//static SDL_Surface *surface = NULL;

int draw(struct chip_t *c8)
{
    static int num = 0;
    if(num == 0)
    {
        for (int i = 0; i < 640 * 47; i++)
        {
            c8->screen[i] = 0xFFFFFFFF;
            printf("NUM 0: %d\n", num);
        }
    }
    else
    {
        for (int i = 0; i < 640 * 480; i++)
        {
            c8->screen[i] = 0xFFFFFFFF;
            if(num < 10)
            printf("NUM 1: %d\n", num);
        }
    }

    uint32_t* pixels = NULL;
    int pitch;  // Pitch = 256 bytes (64 pixels * 4 bytes per pixel)

    SDL_LockTexture(texture, NULL, (void**)&pixels, &pitch);

    int i;
    for (i = 0; i < 2048; i++)
    {
        if (!c8->screen[i])
        {
            pixels[i] = 0x000000FF;
        }
        else
        {
            pixels[i] = 0xFFFFFFFF;
        }
    }

    SDL_UnlockTexture(texture);

    SDL_RenderClear(renderer);

    SDL_RenderCopy(renderer, texture, NULL, NULL);

    SDL_RenderPresent(renderer);

    num++;
    return 0;
}

int is_close_requested()
{
    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
        if (ev.type == SDL_QUIT) {
            return 1;
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
    /*SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF );
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);*/
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
    if (texture != NULL)
    {
        SDL_DestroyTexture(texture);
    }
    SDL_Quit();
}
