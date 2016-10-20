#ifndef SMARTJSON_TYPES_HPP
#define SMARTJSON_TYPES_HPP

#include "sj_config.hpp"

#include <float.h>
#include <stdint.h>
#include <stdlib.h>

namespace mjson
{
    typedef unsigned int SizeType;
    
#if JSON_USE_LARGE_NUMBER
    typedef int64_t     Integer;
    typedef uint64_t    UInteger;
    typedef double      Float;
    const Float DefaultEpsilon = DBL_EPSILON;
#else
    typedef int32_t     Integer;
    typedef int32_t     UInteger;
    typedef float       Float;
    const Float DefaultEpsilon = FLT_EPSILON;
#endif

    enum Type
    {
        T_NULL,
        T_BOOL,
        T_INT,
        T_FLOAT,
        
        T_POINTER,
        T_STRING,
        T_ARRAY,
        T_DICT,
    };

    enum BufferType
    {
        BT_NOT_CARE, // Object use the buffer freely, caller free buffer
        BT_MAKE_COPY, // Object should make a copy of the buffer
        BT_MANAGE, // Object should free this buffer
    };
    
} // end namespace mjson

#endif /* SMARTJSON_TYPES_HPP */
