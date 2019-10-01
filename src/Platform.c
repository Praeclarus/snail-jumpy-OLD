#include <stdio.h>
#include <stdlib.h>

#include <SDL2/SDL_image.h>

#include "Entity.h"
#define PLATFORMS_DEFINITION
#include "Platform.h"
#include "Player.h"
#include "Setup.h"
#include "Vector.h"

void SetupPlatforms()
{
    Platforms = calloc(0, sizeof(*Platforms));
}

void AddPlatform(float left, float bottom, float right, float top)
{
    PlatformCount++;
    Platforms = realloc(Platforms, PlatformCount * sizeof(*Platforms));
    Platforms[PlatformCount - 1] = (typeof(*Platforms)){left, bottom, right, top};
}

void TestPlatforms()
{
    bool fallen   = false;
    
    Vec2* vel = &Entities.loc[0].vel;
    Vec2* pos = &Entities.loc[0].pos;	
	
    int avatarW = Entities.size[0].w;
//    int avatarH = Entities.size[0].h;    

    float aRight  = (pos->x + avatarW);
    float aLeft   = (pos->x);
//    float aTop    = (pos->y + avatarH);
    float aBottom = (pos->y);

    for (unsigned int platformNum = 0; platformNum < PlatformCount; platformNum++)
    {
	typeof(*Platforms) platform = Platforms[platformNum];

	float pRight  = (platform.right);
	float pLeft   = (platform.left);
	float pTop    = (Window.h - platform.top);    
	float pBottom = (Window.h - platform.bottom);
	
	if (!fallen && (
		aRight > pLeft   &&
		pRight > aLeft
		))
	{
	    if (
		aBottom      >= pBottom &&
		pTop         >= aBottom
		)
	    {
		pos->y = pTop;
		vel->y = 0;
		fallen = true;
		Player.falling = false;
 	    }
	    else
	    {
		Player.falling = true;
	    }
	}
	else if (!fallen)
	{
	    Player.falling = true;
	}

    }
}

void DrawPlatforms()
{
    for (unsigned int i = 0; i < PlatformCount; i++)
    {
	SDL_SetRenderDrawColor(Window.ren, 0, 0, 0, 255);
	SDL_RenderDrawRect(Window.ren,
			   &(SDL_Rect){Platforms[i].left,
				   Platforms[i].top,
				   Platforms[i].right-Platforms[i].left,
				   Platforms[i].top-Platforms[i].bottom
				   });
    }
}

void CleanupPlatforms()
{
    free(Platforms);
}
