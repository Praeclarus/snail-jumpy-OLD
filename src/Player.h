#ifndef PLATFORMER_PLAYER_H
#define PLATFORMER_PLAYER_H

#ifdef PLAYER_DEFINITION
#define PLAYER_DEFINITION
#else
#define PLAYER_DEFINITION extern
#endif // PLAYER_DEFINITION

PLAYER_DEFINITION struct {
    uint8_t jumpTime;
    bool    jumpPrev;
    bool falling;
    bool moving;
    bool powerLast;
    unsigned int powerLeft;
} Player;

#endif // PLATFORMER_PLAYER_H
