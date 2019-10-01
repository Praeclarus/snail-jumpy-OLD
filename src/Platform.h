#ifndef PLATFORMER_PLATFORM_H
#define PLATFORMER_PLATFORM_H

#include <stdbool.h>
#include <stdint.h>

#include <SDL2/SDL.h>

#ifdef PLATFORMS_DEFINITION
#define PLATFORMS_DEFINITION
#else
#define PLATFORMS_DEFINITION extern
#endif // PLATFORMS_DEFINITION

PLATFORMS_DEFINITION struct {
    float left, bottom;
    float right, top;
} *Platforms;
PLATFORMS_DEFINITION SDL_Texture* PlatformTexture;
PLATFORMS_DEFINITION unsigned int PlatformCount;

void SetupPlatforms();
void CleanupPlatforms();

void AddPlatform(float left, float bottom, float right, float top);
void DrawPlatforms();
void TestPlatforms();
#endif // PLATFORMER_PLATFORM_H
