#ifndef SNAIL_JUMPY_STATE_H
#define SNAIL_JUMPY_STATE_H

#ifdef GAME_STATE_DEFINITION
#define GAME_STATE_DEFINITION
#else
#define GAME_STATE_DEFINITION extern
#endif // GAME_STATE_DEFINIION

GAME_STATE_DEFINITION struct {
    enum {
	MENU_Start,
	MENU_Pause,
	MENU_Reset,
	MENU_Game
    } menu;
    enum {
	WIN_None,
	WIN_Win,
	WIN_Lose
    } win;
    unsigned int timeLeft;
    bool         pauseMusic;
    bool         playWE;
    unsigned int musicNum;
} Game_state;

#endif // SNAIL_JUMPY_STATE_H
