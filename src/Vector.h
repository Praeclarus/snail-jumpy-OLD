#ifndef PLATFORMER_VECTOR_H
#define PLATFORMER_VECTOR_H

typedef struct TAG_Vec2 {
    float x;
    float y;
} Vec2;

Vec2 AddVec2(Vec2  a, Vec2 b);
void IncVec2(Vec2* a, Vec2 b);
void DecVec2(Vec2* a, Vec2 b);
Vec2 MulVec2(Vec2  a, Vec2 b);

#endif // PLATFORMER_VECTOR_H
