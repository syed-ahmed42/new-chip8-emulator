#include "SDL2/SDL.h"
#include <stdlib.h>
#include <stdint.h>

int start()
{
    SDL_Window *window;

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

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 
            SDL_RENDERER_ACCELERATED
            );

    if (renderer == NULL)
    {
        printf("Failed to create renderer: %s\n", SDL_GetError());
        return 1;
    }
    uint32_t *buffer = malloc((640 * 480) * sizeof(uint32_t));
    uint32_t screen[640][480];
    for (int i = 0; i < 640; i++)
    {
        for (int j = 0; j < 480; j++)
        {
            screen[i][j] = 0x000000FF;
        }
    }

    for (int i = 0; i < 240; i++)
    {
        for (int j = 0; j < 160; j++)
        {
            screen[i][j] = 0xFFFFFF00;
        }
    }

    for (int i = 0; i < 640; i++)
    {
        for (int j = 0; j < 480; j++)
        {
            buffer[(j * 640) + i] = screen[i][j];
        }
    }
    SDL_Texture *texture = SDL_CreateTexture(
            renderer,
            SDL_PIXELFORMAT_RGBA8888,
            SDL_TEXTUREACCESS_TARGET,
            640,
            480
    );
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
    
    SDL_UpdateTexture(texture, NULL, buffer, 640 * sizeof(uint32_t));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);

    int done = 0;

    while (!done)
    {
        SDL_Event event;

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                done = 1;
            }
        }
    }
    if (window)
    {
        SDL_DestroyWindow(window);
    }
    if (renderer)
    {
        SDL_DestroyRenderer(renderer);
    }

    free(buffer);

    SDL_Quit();

    return 0;
}
