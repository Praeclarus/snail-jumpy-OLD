#include <stdbool.h>
#include <stdio.h>

#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

#include "Avatar.h"
#include "Entity.h"
#include "Menu.h"
#include "Player.h"
#include "Points.h"
#include "Setup.h"
#include "Snails.h"
#include "State.h"


#define MENU_TEXT_COLOR  (SDL_Color){10, 10, 60, 255}
#define MENU_FONT_SIZE   20
#define MENU_TEXT_STARTX 20
#define MENU_TEXT_STARTY 250

#define TITLE_WIDTH  128 * 3
#define TITLE_HEIGHT 64  * 3

#define START_MESSAGE "ss [ENTER] to start"
#define QUIT_MESSAGE  "Press [ESCAPE] to quit"

#define TIMER_LONGEVITY 120

#define WE_DELAY_TIME 100


static struct {
    SDL_Texture* title;
    TTF_Font*    font;
    struct {
	Mix_Chunk* win;
	Mix_Chunk* lose;
	Mix_Chunk* start;
    } audio;
} Menu_data;

int SetupMenus()
{
    Game_state.timeLeft = TIMER_LONGEVITY;

    Menu_data.title = IMG_LoadTexture(Window.ren, "../rsrc/Title.png");
    if (Menu_data.title == NULL)
	return -1;
    Menu_data.font = TTF_OpenFont("../rsrc/Press-Start-2P.ttf", MENU_FONT_SIZE);
    if (Menu_data.font == NULL)
	return -2;

    Menu_data.audio.win = Mix_LoadWAV("../rsrc/Sound/WinFX.wav");
    if (Menu_data.audio.win == NULL)
	return -3;

    Menu_data.audio.lose = Mix_LoadWAV("../rsrc/Sound/LoseFX.wav");
    if (Menu_data.audio.win == NULL)
	return -3;

    Menu_data.audio.start = Mix_LoadWAV("../rsrc/Sound/ArenaFX.wav");
    if (Menu_data.audio.win == NULL)
	return -3;


    return 0;
}
      
static int MakeMessages(SDL_Texture** startText, SDL_Texture** quitText)
{
     {
	SDL_Surface* surf = TTF_RenderText_Blended(Menu_data.font, START_MESSAGE, MENU_TEXT_COLOR);
	if (surf == NULL)
	{
	    fprintf(stderr, "TTF_RenderText error: %s \n", TTF_GetError());
	    return -1;
	}
	
	SDL_Texture* text = SDL_CreateTextureFromSurface(Window.ren, surf);
	if (text == NULL)
	{
	    fprintf(stderr, "SDL_CreateTextureFromSurface error: %s \n", SDL_GetError());
	    return -1;
	}
        *startText = text;
	SDL_FreeSurface(surf);
     }

     {
	SDL_Surface* surf = TTF_RenderText_Blended(Menu_data.font, QUIT_MESSAGE, MENU_TEXT_COLOR);
	if (surf == NULL)
	{
	    fprintf(stderr, "TTF_RenderText error: %s \n", TTF_GetError());
	    return -2;
	}
	
	SDL_Texture* text = SDL_CreateTextureFromSurface(Window.ren, surf);
	if (text == NULL)
	{
	    fprintf(stderr, "SDL_CreateTextureFromSurface error: %s \n", SDL_GetError());
	    return -2;
	}
	SDL_FreeSurface(surf);
        *quitText = text;
    }
    return 0;
}
    
static bool EventLoop(SDL_Event* event)
{
    while (SDL_PollEvent(event))
    {
	if (event->type == SDL_QUIT)
	{
	    return true;
	}
	if (event->type == SDL_KEYDOWN)
	{
	    switch (event->key.keysym.sym)
	    {

	    case SDLK_ESCAPE:
		return true;
		break;
	    case SDLK_RETURN:
		if (Game_state.win != WIN_None)
		{
		    PointsCollected = 0;
		    SetEntityPos(0, 70, 320);
		    Game_state.pauseMusic = false;
		    Game_state.win       = WIN_None;
		    Player.powerLast     = true;
		    Player.powerLeft     = 0;
		    Game_state.timeLeft  = TIMER_LONGEVITY;
		    ResetSnails();
		}
		Game_state.pauseMusic = false;
		Mix_PlayChannel(-1, Menu_data.audio.start, 0);
		Avatar_data.alive    = true;
		Game_state.menu   = MENU_Game;
		Game_state.playWE = true;
		
		break;
	    }
	}
    }
    return false;
}

bool MenuStartLoop(SDL_Event* event)
{
    // Event handling
    // --------------
    if (EventLoop(event))
	return true;
    
    // Messages
    // --------
    SDL_Texture* startText;
    SDL_Texture* quitText;
    MakeMessages(&startText, &quitText);

    // Rendering
    // ---------
    SDL_SetRenderDrawColor(Window.ren, 0xcd, 0xe8, 0xcb, 255);
    SDL_RenderClear(Window.ren);

    SDL_RenderCopy(Window.ren, Menu_data.title,
		   NULL,
		   &(SDL_Rect){
		       (Window.w / 2) - (TITLE_WIDTH / 2),
			   0,
			   TITLE_WIDTH,
			   TITLE_HEIGHT
		   });

    int w, h;
    TTF_SizeText(Menu_data.font, START_MESSAGE, &w, &h);
    SDL_RenderCopy(Window.ren, startText, NULL,
		   &(SDL_Rect){
		       MENU_TEXT_STARTX,
			   MENU_TEXT_STARTY,
			   w, h});

    TTF_SizeText(Menu_data.font, QUIT_MESSAGE, &w, &h);
    SDL_RenderCopy(Window.ren, quitText, NULL,
		   &(SDL_Rect){
		       MENU_TEXT_STARTX,
			   MENU_TEXT_STARTY + 30,
			   w, h});
    SDL_DestroyTexture(startText);
    SDL_DestroyTexture(quitText);


    SDL_RenderPresent(Window.ren);

    return false;
}
 
bool MenuPauseLoop(SDL_Event* event)
{
    // Event handling
    // --------------
    if (EventLoop(event))
	return true;
    
    // Messages
    // ---------
    SDL_Texture* startText;
    SDL_Texture* quitText;
    MakeMessages(&startText, &quitText);

    char* message;
    SDL_Color color;
    SDL_Texture* text;
    if (Game_state.win == WIN_Win)
    {
	message = "You Win!";
	color = (SDL_Color){0, 0, 200, 255};
	if (Game_state.playWE)
	{
	    Game_state.pauseMusic = true;
	    Mix_PlayChannel(-1, Menu_data.audio.win, 0);
	    Game_state.playWE = false;
	    SDL_Delay(WE_DELAY_TIME);
	}
    }
    else if (Game_state.win == WIN_Lose)
    {
	message = "You Lose!";
	color = (SDL_Color){200, 0, 0, 255};
	if (Game_state.playWE)
	{
	    Game_state.pauseMusic = true;
	    Mix_PlayChannel(-1, Menu_data.audio.lose, 0);
	    Game_state.playWE = false;
	    SDL_Delay(WE_DELAY_TIME);
	}
    }
    
    
    if (Game_state.win != WIN_None)
    {
	SDL_Surface* surf = TTF_RenderText_Blended(Menu_data.font, message, color);
	if (surf == NULL)
	{
	    fprintf(stderr, "TTF_RenderText error: %s \n", TTF_GetError());
	    return -1;
	}
	
	text = SDL_CreateTextureFromSurface(Window.ren, surf);
	if (text == NULL)
	{
	    fprintf(stderr, "SDL_CreateTextureFromSurface error: %s \n", SDL_GetError());
	    return -1;
	}
    }
    
    
    // Rendering
    // ---------
    SDL_SetRenderDrawColor(Window.ren, 0xcd, 0xe8, 0xcb, 255);
    SDL_RenderClear(Window.ren);

    SDL_RenderCopy(Window.ren, Menu_data.title,
		   NULL,
		   &(SDL_Rect){
		       (Window.w / 2) - (TITLE_WIDTH / 2),
			   0,
			   TITLE_WIDTH,
			   TITLE_HEIGHT
		   });


    int w, h;
    if (Game_state.win != WIN_None)
    {
	TTF_SizeText(Menu_data.font, message, &w, &h);
	SDL_RenderCopy(Window.ren, text, NULL,
		       &(SDL_Rect){
			   MENU_TEXT_STARTX,
			       MENU_TEXT_STARTY,
			       w, h});
    }


    TTF_SizeText(Menu_data.font, START_MESSAGE, &w, &h);
    SDL_RenderCopy(Window.ren, startText, NULL,
		   &(SDL_Rect){
		       MENU_TEXT_STARTX,
			   MENU_TEXT_STARTY + (1 * 30),
			   w, h});

    TTF_SizeText(Menu_data.font, QUIT_MESSAGE, &w, &h);
    SDL_RenderCopy(Window.ren, quitText, NULL,
		   &(SDL_Rect){
		       MENU_TEXT_STARTX,
			   MENU_TEXT_STARTY + (2 * 30),
			   w, h});
    SDL_DestroyTexture(startText);
    SDL_DestroyTexture(quitText);


    SDL_RenderPresent(Window.ren);

    return 0;
}

void CleanupMenus()
{
    SDL_DestroyTexture(Menu_data.title);
    TTF_CloseFont(Menu_data.font);
    
    Mix_FreeChunk(Menu_data.audio.win);
    Mix_FreeChunk(Menu_data.audio.lose);
    Mix_FreeChunk(Menu_data.audio.start);
}
