#ifndef SMARTJSON_CONFIG_HPP
#define SMARTJSON_CONFIG_HPP

#define JSON_USE_LARGE_NUMBER 1

#define JSON_CODE_INLINE 1

#define JSON_SUPPORT_STL_STRING 1

#if JSON_CODE_INLINE
#   define JSON_INLINE inline
#else
#   define JSON_INLINE
#endif

#ifndef JSON_ASSERT
#   if ((defined DEBUG) || defined(_DEBUG))
#include <cassert>
#       define JSON_ASSERT(EXP) assert(EXP)
#   else
#       define JSON_ASSERT(EXP)
#   endif
#endif //JSON_ASSERT

#endif /* SMARTJSON_CONFIG_HPP */
