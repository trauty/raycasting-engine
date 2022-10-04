#pragma once

typedef struct Vec2
{
    float x;
    float y;
}Vec2;

Vec2* newVec2(float x, float y);

typedef struct Vec3
{
    float x;
    float y;
    float z;
}Vec3;

Vec3* newVec3(float x, float y, float z);