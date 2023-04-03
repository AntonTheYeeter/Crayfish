#ifndef CF_MATH_H
#define CF_MATH_H

#include "def.h"

typedef union Vec2f
{
    f32 elements[2];

    struct 
    {
        f32 x;
        f32 y;
    };
} Vec2f;

typedef union Vec3f
{
    f32 elements[3];

    struct 
    {
        f32 x;
        f32 y;
        f32 z;
    };
} Vec3f;

typedef union Vec4f
{
    f32 elements[4];

    struct 
    {
        f32 x;
        f32 y;
        f32 z;
        f32 w;
    };
} Vec4f;

#endif