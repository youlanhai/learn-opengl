#ifndef VERTEX_USAGE_H
#define VERTEX_USAGE_H

#include <string>

enum class VertexUsage
{
    NONE,
    POSITION,
    NORMAL,
    COLOR,
    TANGENT,
    BINORMAL,
    BLENDWEIGHTS,
    BLENDINDICES,
    PSIZE,
    TESSFACTOR,
    POSITIONT,
    FOG,
    DEPTH,
    SAMPLE,
    TEXCOORD0,
    TEXCOORD1,
    TEXCOORD2,
    TEXCOORD3,
    TEXCOORD4,
    TEXCOORD5,
    TEXCOORD6,
    TEXCOORD7,

    MAX_NUM
};

const int VertexUsageMax = int(VertexUsage::MAX_NUM);

VertexUsage vertexAttr2Usage(const std::string & attr);

#endif // VERTEX_USAGE_H
