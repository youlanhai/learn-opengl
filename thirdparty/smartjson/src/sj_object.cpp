#include "sj_object.hpp"
#include "sj_allocator.hpp"

namespace mjson
{
    Object::Object(IAllocator *allocator)
    : allocator_(allocator)
    {
        allocator_->retain();
    }
    
    Object::~Object()
    {
        allocator_->release();
    }
    
    void Object::release()
    {
        JSON_ASSERT(counter_ > 0);
        if(--counter_ <= 0)
        {
            allocator_->freeObject(this);
        }
    }
}
