#include <stdlib.h>
#include "vector.h"

Vec2* newVec2(float x, float y)
{
    Vec2* currentVec2 = malloc(sizeof(Vec2));
    
    currentVec2->x = x;
    currentVec2->y = y;

    return currentVec2;
}

Vec3* newVec3(float x, float y, float z)
{
    Vec3* currentVec3 = malloc(sizeof(Vec3));
    
    currentVec3->x = x;
    currentVec3->y = y;
    currentVec3->z = z;

    return currentVec3;
}