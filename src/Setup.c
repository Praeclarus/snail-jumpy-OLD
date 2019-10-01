#include <stdio.h>
    
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

#define WINDOW_VAR_DEFINED
#include "Setup.h"

#define IMAGE_TYPE_FLAGS (IMG_INIT_PNG)
int InitAll()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
	fprintf(stderr, "SDL_Init error: %s \n", SDL_GetError());
	return -1;
    }

    if ((IMG_Init(IMAGE_TYPE_FLAGS) & IMAGE_TYPE_FLAGS) != IMAGE_TYPE_FLAGS)
    {
	fprintf(stderr, "SDL_Init error: %s \n", SDL_GetError());
	return -1;
    } 

    if (TTF_Init() != 0)
    {
	fprintf(stderr, "TTF_Init error: %s \n", TTF_GetError());
	return -1;
    }

    if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) == -1)
    {
	fprintf(stderr, "MIX_OpenAudio error: %s \n", Mix_GetError());
	return -1;
    }

    return 0;
}


#if SDL_BYTEORDER == SDL_BIG_ENDIAN
#define RMASK 0xff000000
#define GMASK 0x00ff0000
#define BMASK 0x0000ff00
#define AMASK 0x000000ff
#else
#define RMASK 0x000000ff
#define GMASK 0x0000ff00
#define BMASK 0x00ff0000
#define AMASK 0xff000000
#endif

int SetupWindow()
{
    Window.win = SDL_CreateWindow( "Snail Jumpy", 100, 100, 800, 500, SDL_WINDOW_SHOWN);
    if (Window.win == NULL)
    {
	fprintf(stderr, "SDL_CreateWindow error: %s \n", SDL_GetError());
	return -1;
    }
    SDL_GetWindowSize(Window.win, &Window.w, &Window.h);

    Window.ren = SDL_CreateRenderer(Window.win, -1,
				    SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (Window.ren == NULL)
    {
	fprintf(stderr, "SDL_CreateRenderer error: %s \n", SDL_GetError());
	return -1;
    }
    return 0;
}
