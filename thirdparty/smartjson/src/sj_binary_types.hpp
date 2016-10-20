#ifndef SMARTJSON_BINARY_TYPES_HPP
#define SMARTJSON_BINARY_TYPES_HPP

namespace mjson
{
    enum ValueType
    {
        TP_EOF       = 0, //end of file
        TP_NONE      = 1,
        TP_TRUE      = 2,
        TP_FALSE     = 3,
        TP_ZERO      = 4,
        TP_ONE       = 5,

        TP_FLOAT     = 6,
        TP_DOUBLE    = 7,

        TP_INT8      = 8,
        TP_INT16     = 9,
        TP_INT32     = 10,
        TP_INT64     = 11,

        TP_STR0      = 12,
        TP_STR8      = 13,
        TP_STR16     = 14,
        TP_STR32     = 15,

        TP_LIST0     = 16,
        TP_LIST8     = 17,
        TP_LIST16    = 18,
        TP_LIST32    = 19,

        TP_DICT0     = 20,
        TP_DICT8     = 21,
        TP_DICT16    = 22,
        TP_DICT32    = 23,

        TP_MAX       = 24,
    };
}

#endif /* SMARTJSON_BINARY_TYPES_HPP */
