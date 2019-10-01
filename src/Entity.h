#ifndef PLATFORMER_ENTITY_H
#define PLATFORMER_ENTITY_H

#include <SDL2/SDL.h>

#include "Setup.h"
#include "Vector.h"

struct {
    SDL_Texture** texture;
    unsigned int* currFrame;
    struct {
	int w, h;
    } *size;
    struct {
	Vec2 pos;
	Vec2 vel;
    } *loc;
    unsigned int* frameCount;
    unsigned int entityCount;
} Entities;

typedef unsigned int EntityID;

int  SetupEntities();
int  SetupAvatar(SDL_Texture* path, unsigned int w, unsigned int h, unsigned int fCount);
int  AddEntity(char* path, unsigned int w, unsigned int h, unsigned int fCount);
int  AddEntityFromTexture(SDL_Texture* texture, unsigned int w, unsigned int h, unsigned int fCount);
int  RemoveEntity(EntityID ID);

void SetEntityPos(EntityID ID, float x, float y);
void ChangeEntityAnimation(EntityID ID, SDL_Texture* texture, unsigned int fCount);
void AnimateEntity(EntityID ID, unsigned int fOffste, unsigned int counter);

void DrawEntity();
void CleanupEntities();

#endif // PLATFORMER_ENTITY_H
