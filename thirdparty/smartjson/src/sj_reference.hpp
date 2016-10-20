#ifndef SMARTJSON_REFERENCE_HPP
#define SMARTJSON_REFERENCE_HPP

#include "sj_config.hpp"
#include <cstdlib>

#define JSON_DISABLE_COPY_ASSIGN(CLASS) \
    CLASS(const CLASS&); \
    const CLASS& operator=(const CLASS&)

namespace mjson
{
    class Reference
    {
    public:
        Reference();
        virtual ~Reference();
        
        virtual void retain();
        virtual void release();
        
        long getRefCount() const;
        
    protected:
        long        counter_;
    };
    
    size_t maxSize(size_t a, size_t b);
    size_t growCapacity(size_t oldSize, size_t newSize);
}


#if JSON_CODE_INLINE
#include "sj_reference.ipp"
#endif

#endif /* SMARTJSON_REFERENCE_HPP */
