#ifndef SMARTJSON_BINARY_READER_HPP
#define SMARTJSON_BINARY_READER_HPP

#include <cstring>

namespace mjson
{
    class BinaryReader
    {
    public:
        BinaryReader(const char *data, size_t length);
        
        template<typename T>
        T readNumber()
        {
            T v = T(0);
            if(p_ + sizeof(T) <= end_)
            {
                memcpy(&v, p_, sizeof(T));
            }
            p_ += sizeof(T);
            return v;
        }
        
        const char* readBytes(size_t length);
        
        bool empty() const { return p_ >= end_; }
        
        const char* data() const{ return data_; }
        
    private:
        const char*     data_;
        const char*     p_;
        const char*     end_;
    };
}

#endif /* SMARTJSON_BINARY_READER_HPP */
