#ifndef VERTEX_BUFFER_H
#define VERTEX_BUFFER_H

#include "def.h"
#include "core/cf_math.h"

typedef struct Vertex
{
    Vec3f position;
    Vec4f color;
    Vec2f texCoord;
    Vec3f normal;
} Vertex;

#endif