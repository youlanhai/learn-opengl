#ifndef SMARTJSON_STRING_HPP
#define SMARTJSON_STRING_HPP

#include "sj_object.hpp"

#if JSON_SUPPORT_STL_STRING
#include <string>
#endif

namespace mjson
{
    class String : public Object
    {
        JSON_DISABLE_COPY_ASSIGN(String);
    public:
        String(const char *str, size_t size, BufferType type, IAllocator *allocator);
        ~String();
        
        int compare(const char *str) const;
        int compare(const char *str, size_t length) const;
        int compare(const String *p) const;
#if JSON_SUPPORT_STL_STRING
        int compare(const std::string &str) const;
#endif
        
        size_t size() const;
        const char* data() const;
        
        virtual Type type() const;
        
        virtual Object* clone() const;
        virtual Object* deepClone() const;
        
    private:
        char*           str_;
        size_t          size_;
        BufferType      bufferType_;
    };
}

#if JSON_CODE_INLINE
#include "sj_string.ipp"
#endif

#endif /* SMARTJSON_STRING_HPP */
