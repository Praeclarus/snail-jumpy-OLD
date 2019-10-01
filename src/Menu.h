#ifndef SNAIL_JUMPY_MENU_H
#define SNAIL_JUMPY_MENU_H

#include <stdbool.h>

#include <SDL2/SDL.h>

int  SetupMenus();
bool MenuStartLoop(SDL_Event* event);
bool MenuPauseLoop(SDL_Event* event);
void CleanupMenus();

#endif // SNAIL_JUMPY_MENU_H
