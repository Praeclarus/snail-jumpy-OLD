#ifndef PLATFORMER_SETUP_H
#define PLATFORMER_SETUP_H

#include <SDL2/SDL.h>

#ifdef WINDOW_VAR_DEFINITION
extern struct {
    SDL_Window*   win;
    SDL_Renderer* ren;
    int w, h;
} Window;
#else
struct {
    SDL_Window*   win;
    SDL_Renderer* ren;
    int w, h;
} Window;
#endif //WINDOW_VAR_DEFINITION


int InitAll();
int SetupWindow();

#endif // PLATFORMER_SETUP_H
