#include <SDL2/SDL_image.h>

#define AVATAR_DATA_DEFINITION
#include "Avatar.h"
#include "Setup.h"

int SetupAvatarData()
{
    int error = 0;
    if ((Avatar_data.idle  = IMG_LoadTexture(Window.ren, "../rsrc/Avatar_Idle.png")) == NULL)
	error = -1;

    else if ((Avatar_data.death = IMG_LoadTexture(Window.ren, "../rsrc/Avatar_Death.png")) == NULL)
	error = -1;

    else if ((Avatar_data.left  = IMG_LoadTexture(Window.ren, "../rsrc/Avatar_Run_L.png")) == NULL)
	error = -1;

    else if ((Avatar_data.right = IMG_LoadTexture(Window.ren, "../rsrc/Avatar_Run_R.png")) == NULL)
	error = -1;

    else if ((Avatar_data.jumpL = IMG_LoadTexture(Window.ren, "../rsrc/Avatar_Jump_L.png")) == NULL)
	error = -1;

    else if ((Avatar_data.jump  = IMG_LoadTexture(Window.ren, "../rsrc/Avatar_Jump.png")) == NULL)
	error = -1;

    else if ((Avatar_data.jumpR = IMG_LoadTexture(Window.ren, "../rsrc/Avatar_Jump_R.png")) == NULL)
	error = -1;

    if ((Avatar_data.audio.death = Mix_LoadWAV("../rsrc/Sound/DeathFX.wav")) == NULL)
	error = -1;
    else if ((Avatar_data.audio.jump = Mix_LoadWAV("../rsrc/Sound/JumpFX.wav")) == NULL)
	error = -1;
    
    if (error)
    {
	SDL_DestroyTexture(Avatar_data.idle);
	SDL_DestroyTexture(Avatar_data.death);
	SDL_DestroyTexture(Avatar_data.left);
	SDL_DestroyTexture(Avatar_data.right);
	SDL_DestroyTexture(Avatar_data.jumpL);
	SDL_DestroyTexture(Avatar_data.jump);
	SDL_DestroyTexture(Avatar_data.jumpR); 
    }

    return error;
}

void CleanupAvatarData()
{
    SDL_DestroyTexture(Avatar_data.idle);
    SDL_DestroyTexture(Avatar_data.death);
    SDL_DestroyTexture(Avatar_data.left);
    SDL_DestroyTexture(Avatar_data.right);
    SDL_DestroyTexture(Avatar_data.jumpL);
    SDL_DestroyTexture(Avatar_data.jump);
    SDL_DestroyTexture(Avatar_data.jumpR);     

    Mix_FreeChunk(Avatar_data.audio.jump);
    Mix_FreeChunk(Avatar_data.audio.death);
}
