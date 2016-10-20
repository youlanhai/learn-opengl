
namespace mjson
{
    JSON_INLINE Reference::Reference()
    : counter_(0)
    {}
    
    JSON_INLINE Reference::~Reference()
    {}
    
    JSON_INLINE void Reference::retain()
    {
        ++counter_;
    }
    
    JSON_INLINE void Reference::release()
    {
        JSON_ASSERT(counter_ > 0);
        if(--counter_ <= 0)
        {
            delete this;
        }
    }
    
    JSON_INLINE long Reference::getRefCount() const
    {
        return counter_;
    }
    
    JSON_INLINE size_t maxSize(size_t a, size_t b)
    {
        return a > b ? a : b;
    }
    
    JSON_INLINE size_t growCapacity(size_t oldSize, size_t newSize)
    {
        return maxSize(maxSize(oldSize << 1, 4), newSize);
    }
}
