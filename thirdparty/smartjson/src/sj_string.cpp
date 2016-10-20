#include "sj_string.hpp"
#include "sj_allocator.hpp"

#if !JSON_CODE_INLINE
#include "sj_string.ipp"
#endif

namespace mjson
{
    static char buffer[1] = {0};

    String::String(const char *str, size_t size, BufferType type, IAllocator *allocator)
    : Object(allocator)
    , str_(buffer)
    , size_(size)
    , bufferType_(BT_NOT_CARE)
    {
        if(type == BT_NOT_CARE || type == BT_MANAGE)
        {
            str_ = const_cast<char*>(str);
            bufferType_ = type;
        }
        else if(type == BT_MAKE_COPY)
        {
            if(size_ > 0)
            {
                bufferType_ = BT_MANAGE;
                str_ = (char *)allocator_->malloc(size + 1);
                memcpy(str_, str, size);
                str_[size] = 0;
            } 
        }
        else
        {
            JSON_ASSERT(0 && "shouldn't reach here.");
        }
    }

    String::~String()
    {
        if(bufferType_ == BT_MANAGE)
        {
            allocator_->free(str_);
        }
    }
}
