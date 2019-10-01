#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "Avatar.h"
#include "Entity.h"
#include "GameLoop.h"
#include "Menu.h"
#include "Platform.h"
#define PLAYER_DEFINITION
#include "Player.h"
#include "Points.h"
#include "Setup.h"
#include "Snails.h"
#define GAME_STATE_DEFINITION
#include "State.h"
#include "Vector.h"


#define FPS_COUNT        60    
#define BACKGROUND_SCALE 2.0f


static struct {
    Mix_Music* music;
    Mix_Music* powerMusic;
} Audio;

int main(int argc, char* argv[])
{
    if (InitAll())
	return -1;
    if (SetupWindow())
	return -1;

    if (SetupEntities())
	return -1;
    
    // Player
    if (SetupAvatarData())
	return -1;
    SetupAvatar(Avatar_data.idle, 32, 64, 6);
    Player.moving    = false;
    Player.powerLeft = 0;
    Player.powerLast = true;

    // Menus & game loop
    if (SetupMenus())
	return -1;
    if (SetupGameLoop())
	return -1;

    
    // Platforms
    SetupPlatforms();

    // 0 SNAIL
    AddPlatform(20  * BACKGROUND_SCALE, 40  * BACKGROUND_SCALE,
		90  * BACKGROUND_SCALE, 30  * BACKGROUND_SCALE);
    // 1
    AddPlatform(130 * BACKGROUND_SCALE, 60  * BACKGROUND_SCALE,
		170 * BACKGROUND_SCALE, 50  * BACKGROUND_SCALE);
    // 2 SNAIL
    AddPlatform(220 * BACKGROUND_SCALE, 80  * BACKGROUND_SCALE,
		300 * BACKGROUND_SCALE, 70  * BACKGROUND_SCALE);
    // 3 SNAIL
    AddPlatform(60  * BACKGROUND_SCALE, 110 * BACKGROUND_SCALE,
		140 * BACKGROUND_SCALE, 100 * BACKGROUND_SCALE);
    // 4
    AddPlatform(310 * BACKGROUND_SCALE, 130 * BACKGROUND_SCALE,
		330 * BACKGROUND_SCALE, 120 * BACKGROUND_SCALE);
    // 5
    AddPlatform(30  * BACKGROUND_SCALE, 160 * BACKGROUND_SCALE,
		80  * BACKGROUND_SCALE, 150 * BACKGROUND_SCALE);
    // 6 SNAIL
    AddPlatform(130 * BACKGROUND_SCALE, 160 * BACKGROUND_SCALE,
		250 * BACKGROUND_SCALE, 140 * BACKGROUND_SCALE);
    // 7
    AddPlatform(330 * BACKGROUND_SCALE, 180 * BACKGROUND_SCALE,
		390 * BACKGROUND_SCALE, 170 * BACKGROUND_SCALE);
    // 8
    AddPlatform(90  * BACKGROUND_SCALE, 230 * BACKGROUND_SCALE,
		110 * BACKGROUND_SCALE, 220 * BACKGROUND_SCALE);
    // 9
    AddPlatform(170 * BACKGROUND_SCALE, 230 * BACKGROUND_SCALE,
		200 * BACKGROUND_SCALE, 220 * BACKGROUND_SCALE);
    // 10 SNAIL
    AddPlatform(270 * BACKGROUND_SCALE, 230 * BACKGROUND_SCALE,
		350 * BACKGROUND_SCALE, 220 * BACKGROUND_SCALE);

    // Points
    if (SetupPoints())
    	return -1;
    GenerateCoins(5);


    // Snails
    if (SetupSnails(4, (EntityID[]){2, 3, 6, 10}))
    {
    	return -1;
    }

    // State
    Game_state.menu = MENU_Start;
    Game_state.win  = WIN_Win;
    Game_state.playWE = true;
    
    // Setup Music
    Audio.music = Mix_LoadMUS("../rsrc/Sound/Music.wav");
    if (Audio.music == NULL)
    {
	fprintf(stderr, "Mix_LoadWAV error: %s \n", Mix_GetError());
	return -1;
    }
    Audio.powerMusic = Mix_LoadMUS("../rsrc/Sound/PowerMusic.wav");
    if (Audio.powerMusic == NULL)
    {
	fprintf(stderr, "Mix_LoadWAV error: %s \n", Mix_GetError());
	return -1;
    }


    
    // Game loop
    unsigned int counter = 0;
    bool quit = false;
    SDL_Event event;
    while (!quit)
    {
	// Music
	// -----
	if (Game_state.pauseMusic)
	    Mix_HaltMusic();
	else
	{
	    if (Player.powerLeft > 0 && !Player.powerLast)
		Mix_PlayMusic(Audio.powerMusic, -1);
	    else if (!(Player.powerLeft > 0) && Player.powerLast) 
		Mix_PlayMusic(Audio.music, -1);
	} 

	Player.powerLast = (Player.powerLeft);

	// Loops
	// -----
	switch (Game_state.menu)
	{
	case MENU_Start:
	    quit = MenuStartLoop(&event);
	    break;
	case MENU_Game:
	    GameLoop(&counter, &event);
	    break;
	case MENU_Reset:
	case MENU_Pause:
	    quit = MenuPauseLoop(&event);
	    break;
	}
   	SDL_Delay(1000/FPS_COUNT);

    }

    // Cleanup
    CleanupAvatarData();
    CleanupPlatforms();
    
    CleanupPoints();
    CleanupSnails();
    CleanupEntities();
    
    CleanupGameLoop();
    CleanupMenus();

    Mix_FreeMusic(Audio.music);
    Mix_FreeMusic(Audio.powerMusic);

    SDL_DestroyRenderer(Window.ren);
    SDL_DestroyWindow(Window.win);
    SDL_Quit();
    
    return 0;
}

