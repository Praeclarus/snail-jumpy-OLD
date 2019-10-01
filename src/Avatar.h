#ifndef SNAIL_JUMPY_AVATAR_H
#define SNAIL_JUMPY_AVATAR_H

#include <stdbool.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#ifdef  AVATAR_DATA_DEFINITION
#define AVATAR_DATA_DEFINITION
#else
#define AVATAR_DATA_DEFINITION extern
#endif // AVATAR_DATA_DEFINITION

AVATAR_DATA_DEFINITION struct {
    bool         alive;
    SDL_Texture* idle;
    SDL_Texture* death;
    SDL_Texture* left;    
    SDL_Texture* right;    
    SDL_Texture* jumpL;    
    SDL_Texture* jump;    
    SDL_Texture* jumpR;    
    struct audio {
	Mix_Chunk* death;
	Mix_Chunk* jump;
    } audio; 
} Avatar_data;

int  SetupAvatarData();
void CleanupAvatarData();

#endif // SNAIL_JUMPY_AVATAR_H
