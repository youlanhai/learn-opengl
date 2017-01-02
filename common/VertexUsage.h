#ifndef VERTEX_USAGE_H
#define VERTEX_USAGE_H

#include <string>

enum class VertexUsage
{
    POSITION,
    NORMAL,
    TANGENT,
    TEXCOORD0,
    TEXCOORD1,
    TEXCOORD2,
    TEXCOORD3,
    TEXCOORD4,
    COLOR,
	BLENDWEIGHTS,
	BLENDINDICES,

    NONE,
    MAX_NUM
};

const int VertexUsageMax = int(VertexUsage::MAX_NUM);

VertexUsage vertexAttr2Usage(const std::string & attr);
void bindProgramAttribLocation(uint32_t program);

#endif // VERTEX_USAGE_H
