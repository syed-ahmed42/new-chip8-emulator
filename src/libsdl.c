#include <stdlib.h>
#include <stdint.h>

#include "libsdl.h"

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static SDL_Texture *texture = NULL;

static void clean_up()
{
    if (window != NULL)
    {
        SDL_DestroyWindow(window);
        window = NULL;
    }
    if (renderer != NULL)
    {
        SDL_DestroyRenderer(renderer);
        renderer = NULL;
    }
    if (texture != NULL)
    {
        SDL_DestroyTexture(texture);
        texture = NULL;
    }
    SDL_Quit();
}

static void draw(Uint32 *from, Uint32 *to)
{
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++)
    {
        to[i] = from[i];
    }
}

void render_display(struct chip_t *cpu)
{
    void *pixels;
    int pitch;

    SDL_LockTexture(texture, NULL, &pixels, &pitch);
    draw(cpu->screen, (Uint32 *) pixels);
    SDL_UnlockTexture(texture);

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
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
    SDL_Init(SDL_INIT_EVERYTHING);

    window = SDL_CreateWindow(
        "AN SDL WINDOW",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        640,
        480,
        SDL_WINDOW_SHOWN
    );
    if (window == NULL)
    {
        clean_up();
        return 1;
    }

    renderer = SDL_CreateRenderer(window, -1, 
            SDL_RENDERER_ACCELERATED
            );
    if (renderer == NULL)
    {
        clean_up();
        return 1;
    }

    texture = SDL_CreateTexture(
            renderer,
            SDL_PIXELFORMAT_RGBA8888,
            SDL_TEXTUREACCESS_STREAMING,
            640,
            480
    );
    if (texture == NULL)
    {
        clean_up();
        return 1;
    }

    return 0;
}


