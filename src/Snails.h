#ifndef SNAIL_JUMPY_SNAILS_H
#define SNAIL_JUMPY_SNAILS_H


#include "Entity.h"


int  SetupSnails(unsigned int snailCount, unsigned int* pNums);
void TestSnails(bool* quit);
void AnimateSnails(unsigned int counter);
void DrawSnails();
void CleanupSnails();
void ResetSnails();

#endif // SNAIL_JUMPY_SNAILS_H
