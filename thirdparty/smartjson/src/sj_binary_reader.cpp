#include "sj_binary_reader.hpp"

namespace mjson
{
    BinaryReader::BinaryReader(const char *data, size_t length)
    : data_(data)
    , p_(data)
    , end_(data + length)
    {
        
    }

    const char* BinaryReader::readBytes(size_t length)
    {
        if(p_ + length <= end_)
        {
            const char *ret = p_;
            p_ += length;
            return ret;
        }
        else
        {
            p_ = end_;
            return 0;
        }
    }
}
