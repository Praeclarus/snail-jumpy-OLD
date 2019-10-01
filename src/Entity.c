#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <SDL2/SDL_image.h>

#include "Entity.h"

#define ENTITIES_OFFSET(num, name) ((void*) ((char*)Entities) + num * offsetof(Entities, name) )

static struct {
    EntityID*    ids;
    unsigned int count;
} Dead = {NULL, 0};
    
int SetupEntities()
{
    Entities.texture     = NULL;
    Entities.currFrame   = NULL;
    Entities.size        = NULL;
    Entities.loc         = NULL;
    Entities.frameCount  = NULL;
    Entities.entityCount = 0;
    return 0;
}

int SetupAvatar(SDL_Texture* texture, unsigned int w, unsigned int h, unsigned int fCount)
{
    AddEntityFromTexture(texture, w, h, fCount);
    return 0;
}

int AddEntity(char* path, unsigned int w, unsigned int h, unsigned int fCount)
{
    SDL_Texture* tex = IMG_LoadTexture(Window.ren, path);
    if (tex == NULL)
    {
    	fprintf(stderr, "IMG_LoadTexture error: %s \n", IMG_GetError());
    	return -1;
    }

    AddEntityFromTexture(tex, w, h, fCount);
    
    return Entities.entityCount-1;
}

int AddEntityFromTexture(SDL_Texture* texture, unsigned int w, unsigned int h, unsigned int fCount)
{
    EntityID id;
    if (Dead.count)
    {
	id = Dead.ids[0];
    }
    else
    {
	Entities.entityCount++;

	Entities.texture = realloc(Entities.texture,
				   Entities.entityCount * sizeof(*(Entities.texture)));

	Entities.currFrame = realloc(Entities.currFrame,
				     Entities.entityCount * sizeof(*(Entities.currFrame)));

	Entities.size = realloc(Entities.size,
				Entities.entityCount * sizeof(*(Entities.size)));

	Entities.loc        = realloc(Entities.loc,
				      Entities.entityCount * sizeof(*(Entities.loc)));

	Entities.frameCount = realloc(Entities.frameCount,
				      Entities.entityCount * sizeof(*(Entities.frameCount)));

    
	id = Entities.entityCount - 1;
    }
    
    Entities.currFrame[id] = 0;

    Entities.texture[id]    = texture;
    Entities.frameCount[id] = fCount;
    Entities.size[id].w     = w;
    Entities.size[id].h     = h;
    Entities.loc[id].pos    = (Vec2){0.0f, 0.0f};
    Entities.loc[id].vel    = (Vec2){0.0f, 0.0f};
    
    return Entities.entityCount-1;
}

int RemoveEntity(EntityID ID)
{
    if (!(ID <= Entities.entityCount))
	return -1;

    Dead.count++;
    Dead.ids = realloc(Dead.ids, Dead.count * sizeof(EntityID));
    Dead.ids[Dead.count - 1] = ID;
    
    return 0;
}

void DrawEntity(EntityID entity)
{
    int frameX = Entities.currFrame[entity] * Entities.size[entity].w;
    int frameY = 0;
    SDL_RenderCopy(Window.ren, Entities.texture[entity],
		   &(SDL_Rect){frameX, frameY,
			   Entities.size[entity].w, Entities.size[entity].h},
		   &(SDL_Rect){Entities.loc[entity].pos.x,
			   Window.h - ( Entities.loc[entity].pos.y + Entities.size[0].h ),
			   Entities.size[entity].w, Entities.size[entity].h});
}

void SetEntityPos(EntityID ID, float x, float y)
{
    Entities.loc[ID].pos = (Vec2){x, y};
}

void ChangeEntityAnimation(EntityID ID, SDL_Texture* texture, unsigned int fCount)
{
    Entities.texture[ID]    = texture;
    Entities.frameCount[ID] = fCount;
}

void AnimateEntity(EntityID ID, unsigned int fOffset, unsigned int counter)
{
    if ((counter % fOffset) == 0)
    {
	Entities.currFrame[ID]++;
	if (Entities.currFrame[ID] > Entities.frameCount[ID] - 1)
	{
	    Entities.currFrame[ID] = 0;
	}

    }
}

void CleanupEntities()
{
    free(Entities.texture);
    free(Entities.currFrame);
    free(Entities.size);
    free(Entities.loc);
    free(Entities.frameCount);
}
