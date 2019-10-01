#include "Vector.h"

Vec2 AddVec2(Vec2 a, Vec2 b)
{
    return (Vec2){a.x + b.x, a.y + a.y};
}

void IncVec2(Vec2* a, Vec2 b)
{
    a->x += b.x;
    a->y += b.y;
}

void DecVec2(Vec2* a, Vec2 b)
{
    a->x -= b.x;
    a->y -= b.y;
}

Vec2 MulVec2(Vec2 a, Vec2 b)
{
    return (Vec2){a.x * b.x, a.y * a.y};
}
