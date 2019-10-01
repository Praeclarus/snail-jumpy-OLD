#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

#include "Platform.h"
#include "Player.h"
#include "Points.h"
#include "Setup.h"
#define  SNAILS_DEFINITION
#include "Snails.h"
#include "Entity.h"
#include "Vector.h"

#define SNAIL_SPEED       0.35f

#define SNAIL_HEIGHT      32
#define SNAIL_WIDTH       32

#define SNAIL_FRAMES       5
#define SNAIL_DEATH_FRAMES 9
#define SNAIL_FPS_OFFSET   15

static struct {
    int           count;
    EntityID*     IDs;
    unsigned int* pNums;
    // false -> left 
    // true  -> right
    bool*         states; 
    bool*         queue;
    bool*         dead;
} Snails;


static struct {
    SDL_Texture* deathAnim;
    SDL_Texture* lAnim;
    SDL_Texture* rAnim;
    struct {
	Mix_Chunk* ko;
    } audio;
} Snail_data;

int SetupSnails(unsigned int snailCount, unsigned int* pNums)
{
    Snail_data.deathAnim = IMG_LoadTexture(Window.ren, "../rsrc/Snail_KO.png");
    if (Snail_data.deathAnim == NULL)
    {
	return -1;
    }
    Snail_data.lAnim = IMG_LoadTexture(Window.ren, "../rsrc/Snail_L.png");
    if (Snail_data.lAnim == NULL)
    {
	return -1;
    }
    Snail_data.rAnim = IMG_LoadTexture(Window.ren, "../rsrc/Snail_R.png");    
    if (Snail_data.rAnim == NULL)
    {
	return -1;
    }

    Snails.count  = snailCount;
    Snails.IDs    = calloc(snailCount,  sizeof(EntityID));
    Snails.pNums  = calloc(snailCount,  sizeof(unsigned int));    
    Snails.states = calloc(snailCount,  sizeof(bool));
    Snails.queue  = calloc(snailCount,  sizeof(bool));    
    Snails.dead   = calloc(snailCount,  sizeof(bool));    
    for (unsigned int i = 0; i < snailCount; i++)
    {
	Snails.states[i] = false;
	Snails.dead[i] = false;
	Snails.pNums[i]  = pNums[i];	
	Snails.IDs[i]    = AddEntityFromTexture(Snail_data.lAnim, SNAIL_WIDTH, SNAIL_HEIGHT, SNAIL_FRAMES);
	typeof(*Platforms) platform = Platforms[pNums[i]];
	SetEntityPos(Snails.IDs[i],
		     ( (platform.right - platform.left) / 2) + platform.left,
		     Window.h - (platform.top + SNAIL_HEIGHT
 ));
    }

    Snail_data.audio.ko = Mix_LoadWAV("../rsrc/Sound/SnailKOFX.wav");
    if (Snail_data.audio.ko == NULL)
    {
	fprintf(stderr, "Mix_LoadWAV error: %s \n", Mix_GetError());
	return -1;
    }    

    return 0;
}

static bool Intersects(Vec2 a, Vec2 aSize, Vec2 b, Vec2 bSize) {
  return (fabs(a.x - b.x) * 2 < (aSize.x + bSize.x)) && 
        (fabs(a.y - b.y) * 2 < (aSize.y + bSize.y));

}

void TestSnails(bool* quit)
{
    for (unsigned int i = 0; i < Snails.count; i++)
    {
	EntityID id = Snails.IDs[i];
	float aLeft   = Entities.loc[0].pos.x;
	float aBottom = Entities.loc[0].pos.y;
	
	float cLeft   = Entities.loc[id].pos.x;
	float cBottom = Entities.loc[id].pos.y;

	if ((Intersects((Vec2){cLeft, cBottom + (SNAIL_HEIGHT / 2)}, (Vec2){ SNAIL_WIDTH,  SNAIL_HEIGHT/2 },
		       (Vec2){aLeft, aBottom}, (Vec2){ Entities.size[0].w, Entities.size[0].h })) && 
	    !Snails.dead[i] &&
	    !Snails.queue[i])
	{
	    if (Player.powerLeft > 0)
	    {
		Player.powerLeft = 0;
		Mix_PlayChannel(-1, Snail_data.audio.ko, 0);
		Snails.queue[i] = true;
		Entities.currFrame[Snails.IDs[i]] = 0;
		ChangeEntityAnimation(Snails.IDs[i],
				      Snail_data.deathAnim,
				      SNAIL_DEATH_FRAMES);
	    }
	    else
	    {
		*quit = true;
	    }
	    break;
	}
    }
    if (Player.powerLeft > 0)
    {
	Player.powerLeft--;
    }
}

void AnimateSnails(unsigned int counter)
{
    for (unsigned int i = 0; i < Snails.count; i++)
    {
	if (Snails.dead[i])
	    continue;

	EntityID id = Snails.IDs[i];

	if ((counter % SNAIL_FPS_OFFSET) == 0) 
	{
	    Entities.currFrame[id]++;
	    if (Entities.currFrame[id] > Entities.frameCount[id] - 1)
	    {
		Entities.currFrame[id] = 0;
		if (Snails.queue[i])
		{
		    Snails.dead[i] = true;
		    if (Snails.states[i])
			ChangeEntityAnimation(id, Snail_data.rAnim, SNAIL_FRAMES);
		    else
			ChangeEntityAnimation(id, Snail_data.lAnim, SNAIL_FRAMES);
		}
	    }
	}

	if (Snails.queue[i])
	    continue;

	if (Snails.states[i])
	{
	    IncVec2(&Entities.loc[id].pos, (Vec2){SNAIL_SPEED, 0.0f});
	}
	else
	{
	    IncVec2(&Entities.loc[id].pos, (Vec2){-SNAIL_SPEED, 0.0f});
	}
	

	if (Entities.loc[id].pos.x < Platforms[Snails.pNums[i]].left)
	{
	    Snails.states[i] = true;
	    ChangeEntityAnimation(id, Snail_data.rAnim, SNAIL_FRAMES);
	}
	else if (Entities.loc[id].pos.x + SNAIL_WIDTH > Platforms[Snails.pNums[i]].right)
	{
	    Snails.states[i] = false;
	    ChangeEntityAnimation(id, Snail_data.lAnim, SNAIL_FRAMES);
	}
    }
}

void DrawSnails()
{
    for (unsigned int i = 0; i < Snails.count; i++)
    {
	if (!Snails.dead[i])
	    DrawEntity(Snails.IDs[i]);
    }
}

void CleanupSnails()
{
    SDL_DestroyTexture(Snail_data.deathAnim);
    SDL_DestroyTexture(Snail_data.lAnim);
    SDL_DestroyTexture(Snail_data.rAnim);

    Mix_FreeChunk(Snail_data.audio.ko);

    free(Snails.IDs);
    free(Snails.pNums);
    free(Snails.states);
    free(Snails.queue);
    free(Snails.dead);
}

void ResetSnails()
{
    for (unsigned int i = 0; i < Snails.count; i++)
    {
	Snails.queue[i] = false;
	Snails.dead[i]  = false;
	if (Snails.states[i])
	    ChangeEntityAnimation(Snails.IDs[i], Snail_data.rAnim, SNAIL_FRAMES);
	else
	    ChangeEntityAnimation(Snails.IDs[i], Snail_data.lAnim, SNAIL_FRAMES);
    }
}
