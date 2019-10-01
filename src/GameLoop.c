#include <math.h>
#include <stdbool.h>
#include <stdio.h>

#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

#include "Avatar.h"
#include "Entity.h"
#include "Platform.h"
#include "Player.h"
#include "Points.h"
#include "Setup.h"
#include "Snails.h"
#include "State.h"


#define GRAVITY_STRENGTH    2.0f
#define JUMP_STRENGTH       10.5f
#define JUMP_AMOUNT         4
#define MOVEMENT_SPEED      0.7f
#define JUMP_MOVEMENT_SPEED 1.2f
#define MOVEMENT_DRAG       0.15f

#define TIME_FONT_SIZE 20
#define TIME_COLOR     (SDL_Color){20, 20, 20, 255}

#define DEATH_PAUSE_TIME 50


static SDL_Texture* Background; 
static TTF_Font*    TimeFont;


int SetupGameLoop()
{
    Background = IMG_LoadTexture(Window.ren, "../rsrc/Background.png");
    if (Background == NULL)
    {
	fprintf(stderr, "IMG_LoadTexture error: %s \n", IMG_GetError());
	return -1;
    }
    TimeFont   = TTF_OpenFont("../rsrc/Press-Start-2P.ttf", TIME_FONT_SIZE);
    if (Background == NULL)
    {
	fprintf(stderr, "IMG_OpenFont error: %s \n", TTF_GetError());
	return -1;
    }

    return 0;
}

void GameLoop(unsigned int* counter, SDL_Event* event)
{
    // Event handling
    // --------------

    // Single press
    while (SDL_PollEvent(event) != 0)
    {
	if (event->type == SDL_QUIT)
	{
		Game_state.menu = MENU_Pause;	    
	}
	else if (event->type == SDL_KEYDOWN)
	{
	    switch (event->key.keysym.sym)
	    {
	    case SDLK_ESCAPE:
		Game_state.menu = MENU_Pause;
		break;
	    }
	}
    }
	
    // Holding down
    const Uint8* keyState = SDL_GetKeyboardState(NULL);
	
    // Movement
    {

	int dir;
	if (keyState[SDL_SCANCODE_LEFT] || keyState[SDL_SCANCODE_A])
	{
	    ChangeEntityAnimation(0, Avatar_data.left, 8);
	    dir = -1;
	}
	else if (keyState[SDL_SCANCODE_RIGHT] || keyState[SDL_SCANCODE_D])
	{
	    ChangeEntityAnimation(0, Avatar_data.right, 8);
	    dir = 1;
	}
	else
	{
	    if (Player.moving)
		Entities.currFrame[0] = 0;
		
	    ChangeEntityAnimation(0, Avatar_data.idle, 6);

	    Entities.loc[0].vel.x -= MOVEMENT_DRAG * Entities.loc[0].vel.x;
		
	    dir = 0;
	}
	Player.moving = keyState[SDL_SCANCODE_LEFT] | keyState[SDL_SCANCODE_RIGHT] |
	    keyState[SDL_SCANCODE_A] | keyState[SDL_SCANCODE_D];
	    
	float speed;
	if (!Player.jumpPrev)
	    speed = MOVEMENT_SPEED;
	else
	    speed = JUMP_MOVEMENT_SPEED;
		
	Entities.loc[0].vel.x += dir * speed -
	    (MOVEMENT_DRAG * Entities.loc[0].vel.x);

    }
	
	
    // Jumping
    if (keyState[SDL_SCANCODE_SPACE])
    {
	if (Player.jumpTime == 0)
	    Mix_PlayChannel(-1, Avatar_data.audio.jump, 0);
	Player.jumpTime++;
	if (Player.jumpTime < JUMP_AMOUNT)
	{
	    if (!(fabs(Entities.loc[0].vel.x) < 0.5f))
	    {
		if (Entities.loc[0].vel.x > 0.0f)
		{
		    Entities.currFrame[0] = 0;
		    ChangeEntityAnimation(0, Avatar_data.jumpR, 1);
		}
		else if (Entities.loc[0].vel.x < 0.0f)
		{
		    Entities.currFrame[0] = 0;
		    ChangeEntityAnimation(0, Avatar_data.jumpL, 1);
		}
	    }
	    else
	    {
		Entities.currFrame[0] = 0;
		ChangeEntityAnimation(0, Avatar_data.jump, 1);
	    }
	    Vec2 jumpVel = (Vec2) {0.0f, 0.0f};
	    jumpVel = (Vec2){0.0f, JUMP_STRENGTH};

	    Vec2 drag = MulVec2((Vec2){0.0f, MOVEMENT_DRAG}, Entities.loc[0].vel);
	    DecVec2(&jumpVel, drag);

	    IncVec2(&Entities.loc[0].vel, jumpVel);
	}
	if (Player.falling == false && !Player.jumpPrev)
	{
	    Player.jumpTime = 0;
	}
    }
    Player.jumpPrev = keyState[SDL_SCANCODE_SPACE];
	
    // Update
    // ------
    IncVec2(&Entities.loc[0].pos, Entities.loc[0].vel);

    // Testing
    TestPlatforms();
    {
    	bool hit = false;
    	TestSnails(&hit);
    	if (hit)
    	{

    	    Avatar_data.alive = false;
    	    Game_state.menu   = MENU_Reset;
    	    Game_state.win    = WIN_Lose;
    	}
    }
    TestPoints();
	
    // Gravity
    if (Player.falling == true)
	Entities.loc[0].vel.y -= GRAVITY_STRENGTH + MOVEMENT_DRAG * Entities.loc[0].vel.y;

    // Prevent endless falling
    if (Entities.loc[0].pos.y < -250.0f)
    {
	Mix_PlayChannel(-1, Avatar_data.audio.death, 0);
	SDL_Delay(DEATH_PAUSE_TIME);
	Game_state.pauseMusic = true;
	Game_state.menu = MENU_Reset;
	Game_state.win  = WIN_Lose;
    }

    // Animation
    // ---------
    (*counter)++;

    // Avatar animation
    if (Player.moving)
    {
	AnimateEntity(0, 5, *counter);
    }
    else
    { 
	AnimateEntity(0, 20, *counter);
    }

    AnimateCoins(*counter);
    AnimateSnails(*counter);

    if (Game_state.timeLeft > 0)
    {
	if ((*counter % 60) == 0)
	    Game_state.timeLeft--;
    }
    else
    {
	Game_state.pauseMusic = true;
	Game_state.win  = WIN_Lose;
	Game_state.menu = MENU_Reset;
    }

    // Time
    static char  timeMsg[10];
    SDL_Texture* timeText;
    int TTW, TTH;
    {
	sprintf(timeMsg, "Time: %u:%u", Game_state.timeLeft / 60, Game_state.timeLeft % 60);
	SDL_Surface* surf = TTF_RenderText_Blended(TimeFont, timeMsg, TIME_COLOR);
	if (surf == NULL)
	{
	    fprintf(stderr, "TTF_RenderText error: %s \n", TTF_GetError());
	    return;
	}

	timeText = SDL_CreateTextureFromSurface(Window.ren, surf);
	if (timeText == NULL)
	{
	    fprintf(stderr, "SDL_TextureFromSurface error: %s \n", SDL_GetError());
	    return;
	}
	TTF_SizeText(TimeFont, timeMsg, &TTW, &TTH);
	SDL_FreeSurface(surf);
    }
    
    // Drawing
    // -------
    SDL_RenderClear(Window.ren);
    SDL_RenderCopy(Window.ren, Background, NULL, NULL);

    DrawEntity(0);
    DrawCoins();
    DrawSnails();

    DisplayPoints();
	
    SDL_RenderCopy(Window.ren, timeText, NULL,
		   &(SDL_Rect){
		       (Window.w / 2) - (TTW / 2),
			   10,
			   TTW,
			   TTH
		   });

    SDL_RenderPresent(Window.ren);


    if (!Avatar_data.alive)
    {
	Mix_PlayChannel(-1, Avatar_data.audio.death, 0);
	ChangeEntityAnimation(0, Avatar_data.death, 7);
	Entities.size[0].w    = 64;
	Entities.currFrame[0] = 0;
	for (unsigned int i = 0; i < (Entities.frameCount[0] - 1) * 5; i++)
	{
	    AnimateEntity(0, 5, i);
	    SDL_RenderClear(Window.ren);
	    SDL_RenderCopy(Window.ren, Background, NULL, NULL);

	    DrawEntity(0);
	    DrawCoins();
	    DrawSnails();

	    DisplayPoints();
	
	    SDL_RenderCopy(Window.ren, timeText, NULL,
			   &(SDL_Rect){
			       (Window.w / 2) - (TTW / 2),
				   10,
				   TTW,
				   TTH
				   });

	    SDL_RenderPresent(Window.ren);
	    SDL_Delay(1000/60);
	}
	SDL_Delay(DEATH_PAUSE_TIME);
	Entities.size[0].w = 32;
    }

    SDL_DestroyTexture(timeText);
}

void CleanupGameLoop()
{
    SDL_DestroyTexture(Background);
    TTF_CloseFont(TimeFont);
}
