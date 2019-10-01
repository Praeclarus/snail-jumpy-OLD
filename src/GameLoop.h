#ifndef SNAIL_JUMPY_GAME_LOOP_H
#define SNAIL_JUMPY_GAME_LOOP_H

#include <stdbool.h>
#include <SDL2/SDL.h>

int  SetupGameLoop();
void GameLoop(unsigned int* counter, SDL_Event* event);
void CleanupGameLoop();

#endif // SNAIL_JUMPY_GAME_LOOP_H
