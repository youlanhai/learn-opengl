#ifndef H__RENDER_STATE
#define H__RENDER_STATE

#include "glconfig.h"

namespace ColorMask
{
    const uint32_t A = 0xff << 24;
    const uint32_t R = 0xff << 16;
    const uint32_t G = 0xff << 8;
    const uint32_t B = 0xff << 0;
}

enum class RenderState
{
    CullFace = GL_CULL_FACE,
    Blend = GL_BLEND,
    DepthTest = GL_DEPTH_TEST,
    ScissorTest = GL_SCISSOR_TEST,
    StencilTest = GL_STENCIL_TEST,
};

enum class BlendFun
{
    Zero = GL_ZERO,
    One = GL_ONE,

    SrcColor = GL_SRC_COLOR,
    InvSrcColor = GL_ONE_MINUS_SRC_COLOR,

    DstColor = GL_DST_COLOR,
    InvDstColor = GL_ONE_MINUS_DST_COLOR,

    SrcAlpha = GL_SRC_ALPHA,
    InvSrcAlpha = GL_ONE_MINUS_SRC_ALPHA,

    DstAlpha = GL_DST_ALPHA,
    InvDstAlpha = GL_ONE_MINUS_DST_ALPHA,

    ConstantColor = GL_CONSTANT_COLOR,
    InvConstantColor = GL_ONE_MINUS_CONSTANT_COLOR,

    ConstantAlpha = GL_CONSTANT_ALPHA,
    InvConstantAlpha = GL_ONE_MINUS_CONSTANT_ALPHA,
};

enum class CullFace
{
    Front = GL_FRONT,
    Back = GL_BACK,
    FrontBack = GL_FRONT_AND_BACK,
};

enum class CompareFun
{
    Never = GL_NEVER,
    Less = GL_LESS,
    Equal = GL_LEQUAL,
    LessEqual = GL_EQUAL,
    Greater = GL_GREATER,
    GreaterEqual = GL_GEQUAL,
    NotEqual = GL_NOTEQUAL,
    Always = GL_ALWAYS,
};

enum class StencilOp
{
    Keep = GL_KEEP,
    Zero = GL_ZERO,
    Relpace = GL_REPLACE,
    Incr = GL_INCR,
    IncrWrap = GL_INCR_WRAP,
    Decr = GL_DECR,
    DecrWrap = GL_DECR_WRAP,
    Invert = GL_INVERT,
};

enum class TexType
{
    Tex2D,
    Tex3D,
    TexCubeMap,
};

enum class TextureTarget
{
    Tex2D = GL_TEXTURE_2D,
    TexCubeMap = GL_TEXTURE_CUBE_MAP,
};

enum class TextureParam
{
    MinFilter = GL_TEXTURE_MIN_FILTER,
    MagFilter = GL_TEXTURE_MAG_FILTER,
    WrapU = GL_TEXTURE_WRAP_S,
    WrapV = GL_TEXTURE_WRAP_T,
};

enum class TextureFilter
{
    Near = GL_NEAREST,
    Linear = GL_LINEAR,
    NearMipmapNear = GL_NEAREST_MIPMAP_LINEAR,
    NearMipmapLinear = GL_NEAREST_MIPMAP_LINEAR,
    LinearMipmapNear = GL_LINEAR_MIPMAP_NEAREST,
    LinearMipmapLinear = GL_LINEAR_MIPMAP_LINEAR,
};

enum class TextureQuality
{
    Nearest,
    Linear,
    TwoLinear,
    ThreeLinear,
    FourLinear,
    Anisotropic,
    Default,
};

enum class TextureWrap
{
    Repeat = GL_REPEAT,
    Mirror = GL_MIRRORED_REPEAT,
    Clamp = GL_CLAMP_TO_EDGE,
};

enum class TextureFormat
{
    Unknown         = 0,
    Luminance       = GL_LUMINANCE,
    LuminanceAlpha  = GL_LUMINANCE_ALPHA,
    RGB             = GL_RGB,
    RGBA            = GL_RGBA,
    ALPHA           = GL_ALPHA,
	Depth			= GL_DEPTH_COMPONENT,
	DepthStencil	= GL_DEPTH_STENCIL,

#if defined(USE_PVRTC)
    PVRTC4BPP_A     = GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG,
    PVRTC4BPP       = GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG,
    PVRTC2BPP_A     = GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG,
    PVRTC2BPP       = GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG,
#endif

#if defined(USE_ETC)
    ETC             = GL_ETC1_RGB8_OES,
#endif
};

enum class PrimitiveType
{
    PointList = GL_POINTS,
    LineList = GL_LINES,
    LineStrip = GL_LINE_STRIP,
    LineLoop = GL_LINE_LOOP,
    TriangleList = GL_TRIANGLES,
    TriangleStrip = GL_TRIANGLE_STRIP,
    TriangleFan = GL_TRIANGLE_FAN,
};

enum class BufferType
{
    Vertex = GL_ARRAY_BUFFER,
    Index = GL_ELEMENT_ARRAY_BUFFER,
};

enum class BufferUsage
{
    Static = GL_STATIC_DRAW,
    Dynamic = GL_DYNAMIC_DRAW,
};

enum class IndexType
{
    Index8 = GL_UNSIGNED_BYTE,
    Index16 = GL_UNSIGNED_SHORT,
    Index32 = GL_UNSIGNED_INT
};

#endif //H__RENDER_STATE
