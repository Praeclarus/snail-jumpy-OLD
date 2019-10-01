#ifndef PLATFORMER_POINTS_H
#define PLATFORMER_POINTS_H

#include <stdint.h>

#ifndef POINTSCOLLECTED_DEFINE
#define POINTSCOLLECTED_DEFINE extern
#else
#define POINTSCOLLECTED_DEFINE
#endif // POINTSCOLLECTED_DEFINE

POINTSCOLLECTED_DEFINE unsigned int PointsCollected;

int  SetupPoints();
void FreeCoins();
int  GenerateCoins(uint8_t count);
void AnimateCoins(int frame);
void TestPoints();
void DrawCoins();
int  DisplayPoints();
void CleanupPoints();

#endif // PLATFORMER_POINTS_H
