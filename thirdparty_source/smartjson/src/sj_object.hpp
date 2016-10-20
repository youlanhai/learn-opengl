#ifndef SMARTJSON_OBJECT_HPP
#define SMARTJSON_OBJECT_HPP

#include "sj_reference.hpp"
#include "sj_types.hpp"

namespace mjson
{
    class IAllocator;
    
    class Object : public Reference
    {
        JSON_DISABLE_COPY_ASSIGN(Object);
    public:
        explicit Object(IAllocator *allocator);
        ~Object();
        
        virtual void    release();
        virtual Type    type() const = 0;
        virtual Object* clone() const = 0;
        virtual Object* deepClone() const = 0;
        
        JSON_INLINE IAllocator* getAllocator(){ return allocator_; }
        
    protected:
        IAllocator*     allocator_;
    };
}

#endif /* SMARTJSON_OBJECT_HPP */
