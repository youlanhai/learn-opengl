#ifndef VERTEX_H
#define VERTEX_H

#include "Vector2.h"
#include "Vector3.h"
#include "Color.h"

#define DEF_VERTEX_TYPE(name) static const char * getType(){ return #name; }

struct VertexXYZ
{
    Vector3 position;

    DEF_VERTEX_TYPE(oxyz)
};

struct VertexXYZColor
{
    Vector3 position;
    Color color;

    DEF_VERTEX_TYPE(oxyzc)
};

struct VertexXYZColorUV
{
    Vector3 position;
    Color   color;
    Vector2 uv;

    DEF_VERTEX_TYPE(oxyzcuv)
};

struct VertexXYZN
{
    Vector3 position;
    Vector3 normal;

    DEF_VERTEX_TYPE(oxyzn)
};

struct VertexXYZUV
{
    Vector3 position;
    Vector2 uv;

    DEF_VERTEX_TYPE(oxyzuv)
};

struct VertexXYZNUV
{
    Vector3 position;
    Vector3 normal;
    Vector2 uv;

    DEF_VERTEX_TYPE(oxyznuv)
};

struct MeshVertex
{
	Vector3 position;
	Vector3 normal;
	Vector2 uv;
	Vector3 tangent;

    DEF_VERTEX_TYPE(oxyznuvt)
};

#endif //VERTEX_H
