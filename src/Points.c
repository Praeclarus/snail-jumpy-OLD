#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

#include "Entity.h"
#include "Platform.h"
#include "Player.h"
#define POINTSCOLLECTED_DEFINE
#include "Points.h"
#include "Setup.h"
#include "State.h"


#define COIN_WIDTH              32
#define COIN_HEIGHT             32

#define COIN_FRAMES             6
#define COIN_COLLECT_FRAMES     4

#define COIN_FPS_OFFSET         5
#define COIN_COLLECT_FPS_OFFSET 5

#define POINTS_WIN_COUNT  60
#define POINTS_TEXT_COLOR (SDL_Color){50, 50, 50, 255}

#define POWERUP_FRAMES         8
#define POWERUP_COLLECT_FRAMES 11

#define POWERUP_RANGE  100
#define POWERUP_CHANCE 0.03f

#define POWERUP_LONGEVITY 200


static struct {
    EntityID*    CoinIDs;
    bool*        Powerups;
    bool*        Collect;
    unsigned int CoinCount;
    struct {
	Mix_Chunk* coinCollect;
	Mix_Chunk* powerCollect;
    } audio;
} Coins = { NULL, NULL, NULL, 0 };

static struct {
    TTF_Font*    PointsFont;
    SDL_Texture* CoinAnim;
    SDL_Texture* PowerAnim;
    SDL_Texture* CoinCollectAnim;
    SDL_Texture* PowerCollectAnim;
    SDL_Texture* PowerIcon;
} Drawing;

static struct {
    uint8_t pNum;
    uint8_t pos;
} *CoinPosArray = {NULL};


int SetupPoints()
{
    Drawing.PointsFont = TTF_OpenFont("../rsrc/Press-Start-2P.ttf", 20);
    if (Drawing.PointsFont == NULL)
    {
	fprintf(stderr, "TTF_OpenFont error: %s \n", TTF_GetError());
	return -1;
    }
    Drawing.CoinAnim         = IMG_LoadTexture(Window.ren, "../rsrc/Coin.png");
    Drawing.PowerAnim        = IMG_LoadTexture(Window.ren, "../rsrc/Power_Up.png");
    Drawing.PowerCollectAnim = IMG_LoadTexture(Window.ren, "../rsrc/Power_Up_Collect.png");
    Drawing.PowerIcon        = IMG_LoadTexture(Window.ren, "../rsrc/Power_Up_Icon.png");        
    Drawing.CoinCollectAnim  = IMG_LoadTexture(Window.ren, "../rsrc/Coin_Collect.png");    

    Coins.audio.coinCollect  = Mix_LoadWAV("../rsrc/Sound/CoinFX.wav");
    Coins.audio.powerCollect = Mix_LoadWAV("../rsrc/Sound/PowUpFX.wav");

    srand(time(0));

    return 0;
}

int GenerateCoins(const uint8_t count)
{
    Coins.CoinIDs  = calloc(count, sizeof(EntityID));
    Coins.Collect  = calloc(count, sizeof(bool));
    Coins.Powerups = calloc(count, sizeof(bool));
    for (uint8_t i = 0; i < count; i++)
    {
	Coins.Powerups[i] = false;
	Coins.Collect[i]  = false;
	uint8_t pNum = rand() % (PlatformCount);
	typeof(*Platforms) platform = Platforms[pNum];
	unsigned int range = (platform.right - platform.left) / COIN_WIDTH;
	if (range == 0)
	    continue;
	unsigned int pos = rand() % (range);
	CoinPosArray = realloc(CoinPosArray, (Coins.CoinCount + 1) * sizeof(*CoinPosArray));
	CoinPosArray[i].pNum = pNum;
	CoinPosArray[i].pos  = pos;

	bool taken;
	for (uint8_t j = 0; j < Coins.CoinCount; j++)
	{
	    if (j == i)
		continue;
	    if (CoinPosArray[j].pos == pos && CoinPosArray[j].pNum == pNum)
	    {
		taken = true;
		break;
	    }
	    else
		taken = false;
	}

	if (!taken)
	{
	    Coins.CoinCount++;
	    Coins.CoinIDs[Coins.CoinCount-1] = AddEntityFromTexture(
		Drawing.CoinAnim,
		COIN_WIDTH,
		COIN_HEIGHT,
		6);
	    SetEntityPos(
		Coins.CoinIDs[Coins.CoinCount-1],
		pos * COIN_WIDTH + platform.left,
		Window.h - platform.top - (COIN_HEIGHT / 2));
	}
	else
	    i--;
    }

    return 0;
}

void AnimateCoins(int frame)
{
    if ((frame % COIN_FPS_OFFSET) == 0)
    {
	for (unsigned int i = 0; i < Coins.CoinCount; i++)
	{
	    Entities.currFrame[Coins.CoinIDs[i]]++;


	    if (Entities.currFrame[Coins.CoinIDs[i]] >= Entities.frameCount[Coins.CoinIDs[i]])
	    {

		Entities.currFrame[Coins.CoinIDs[i]] = 0;
		if (!Coins.Collect[i])
		    continue;

		bool taken   = true;
		bool powerup = false;
		unsigned int pNum, pos;
		typeof(*Platforms) platform;
		while (taken)
		{
		    pNum = rand() % (PlatformCount);
		    platform = Platforms[pNum];
		    unsigned int range = (platform.right - platform.left) / COIN_WIDTH;
		    if (range == 0)
			continue;
		    pos = rand() % (range);
		    CoinPosArray[i].pNum  = pNum;
		    CoinPosArray[i].pos  = pos;

		    powerup = (rand() % POWERUP_RANGE) < POWERUP_RANGE * POWERUP_CHANCE;

		    for (uint8_t j = 0; j < Coins.CoinCount - 1; j++)
		    {
			if (j == i)
			    continue;

			taken = ((CoinPosArray[j].pos == pos) && (CoinPosArray[j].pNum == pNum));
			if (taken)
			    break;
		    }

		}
		SetEntityPos(Coins.CoinIDs[i],
			     pos * COIN_WIDTH + platform.left,
			     Window.h - platform.top - (COIN_HEIGHT / 2));
		if (powerup)
		{
		    ChangeEntityAnimation(Coins.CoinIDs[i], Drawing.PowerAnim, POWERUP_FRAMES);
		}
		else
		{
		    ChangeEntityAnimation(Coins.CoinIDs[i], Drawing.CoinAnim, COIN_FRAMES);
		}
		Coins.Powerups[i] = powerup;
		Coins.Collect[i] = false;
	    }
	}
    }

}

void DrawCoins()
{
    for (unsigned int i = 0; i < Coins.CoinCount; i++)
    {
	DrawEntity(Coins.CoinIDs[i]);
    }
}

static bool Intersects(Vec2 a, Vec2 aSize, Vec2 b, Vec2 bSize) {
  return (fabs(a.x - b.x) * 2 < (aSize.x + bSize.x)) &&
         (fabs(a.y - b.y) * 2 < (aSize.y + bSize.y));

}

void TestPoints()
{
    if (PointsCollected >= POINTS_WIN_COUNT)
    {
	Game_state.menu = MENU_Reset;
	Game_state.win  = WIN_Win;
    }
    for (unsigned int i = 0; i < Coins.CoinCount; i++)
    {
	float aLeft   = Entities.loc[0].pos.x;
	float aBottom = Entities.loc[0].pos.y;
	
	float cLeft   = Entities.loc[Coins.CoinIDs[i]].pos.x;
	float cBottom = Entities.loc[Coins.CoinIDs[i]].pos.y;

	if (Intersects((Vec2){cLeft, cBottom}, (Vec2){ COIN_WIDTH,         COIN_HEIGHT },
		       (Vec2){aLeft, aBottom}, (Vec2){ Entities.size[0].w, Entities.size[0].h }) &&
	    !Coins.Collect[i])
	{
	    if (Coins.Powerups[i])
	    {
		Mix_PlayChannel(-1, Coins.audio.powerCollect, 0);
		ChangeEntityAnimation(Coins.CoinIDs[i], Drawing.PowerCollectAnim, POWERUP_COLLECT_FRAMES);
		Player.powerLeft = POWERUP_LONGEVITY;
	    }
	    else
	    {
		Mix_PlayChannel(-1, Coins.audio.coinCollect, 0);
		ChangeEntityAnimation(Coins.CoinIDs[i], Drawing.CoinCollectAnim, COIN_COLLECT_FRAMES);
		PointsCollected++;
	    }
	    Entities.currFrame[Coins.CoinIDs[i]] = 0;
	    Coins.Collect[i] = true;
	    break;
	}
    }
}

int DisplayPoints()
{
    char message[15];
    sprintf(message, "Points: %u", PointsCollected);

    if (Player.powerLeft > 0)
    {
    SDL_RenderCopy(Window.ren, Drawing.PowerIcon,
		   NULL,
		   &(SDL_Rect){
		       Window.w - (3 * COIN_WIDTH),
			   0,
			   3 * COIN_WIDTH,
			   3 * COIN_HEIGHT
			   });
    }
    SDL_Surface* surf = TTF_RenderText_Blended(Drawing.PointsFont, message, POINTS_TEXT_COLOR);
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
    SDL_FreeSurface(surf);

    int w, h;
    TTF_SizeText(Drawing.PointsFont, message, &w, &h);
    SDL_RenderCopy(Window.ren, text, NULL, &(SDL_Rect){10, 10,  w, h});
    
    SDL_DestroyTexture(text);
    return 0;
}

void CleanupPoints()
{
    TTF_CloseFont(Drawing.PointsFont);
    SDL_DestroyTexture(Drawing.CoinAnim);
    SDL_DestroyTexture(Drawing.PowerAnim);
    SDL_DestroyTexture(Drawing.CoinCollectAnim);
    SDL_DestroyTexture(Drawing.PowerCollectAnim);
    SDL_DestroyTexture(Drawing.PowerIcon);
    SDL_DestroyTexture(Drawing.CoinAnim);

    Mix_FreeChunk(Coins.audio.coinCollect);
    Mix_FreeChunk(Coins.audio.powerCollect);

    free(Coins.CoinIDs);
    free(Coins.Powerups);
    free(Coins.Collect);
}
