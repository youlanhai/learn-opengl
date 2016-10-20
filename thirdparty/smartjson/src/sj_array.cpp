#include "sj_array.hpp"
#include "sj_allocator.hpp"

#if !JSON_CODE_INLINE
#include "sj_array.ipp"
#endif

namespace mjson
{
    Array::~Array()
    {
        clear();
        if(capacity_ > 0)
        {
            allocator_->free(begin_);
        }
    }
    
    void Array::reserve(size_t capacity)
    {
        if(capacity > capacity_)
        {
            size_t size = this->size();
            value_type *old = begin_;
            
            begin_ = (value_type*) allocator_->malloc(capacity * sizeof(value_type));
            if(size != 0)
            {
                memcpy(begin_, old, size * sizeof(value_type));
            }
            if(old != 0)
            {
                allocator_->free(old);
            }
            
            end_ = begin_ + size;
            capacity_ = capacity;
        }
    }
    
    void Array::alignedReserve(size_t capacity)
    {
        if(capacity > capacity_)
        {
            size_t newCapacity = growCapacity(capacity_, capacity);
            reserve(newCapacity);
        }
    }
    
    void Array::resize(size_t size)
    {
        alignedReserve(size);
        
        pointer newEnd = begin_ + size;
        if(newEnd > end_)
        {
            for(pointer it = end_; it != newEnd; ++it)
            {
                new (it) value_type();
            }
        }
        else if(newEnd < end_)
        {
            for(pointer p = newEnd; p != end_; ++p)
            {
                p->~value_type();
            }
        }
        
        end_ = newEnd;
    }
    
    void Array::insert(iterator it, const value_type &value)
    {
        JSON_ASSERT(it < end() || it == end());
        
        alignedReserve(size() + 1);
        
        pointer p = begin_ + it.index();
        if(p != end_)
        {
            //move backwards.
            memmove(p + 1, p, (end_ - p) * sizeof(value_type));
        }
        ++end_;
        
        new (p) value_type(value);
    }
    
    void Array::erase(iterator it)
    {
        //NOTICE erase the end is valid.
        JSON_ASSERT(it >= begin() && it <= end());
        if(it != end())
        {
            it->~value_type();
            
            pointer p = begin_ + it.index();
            pointer next = p + 1;
            if(next != end_)
            {
                // move forwards.
                memmove(p, next, (end_ - next) * sizeof(value_type));
            }
            --end_;
        }
    }
    
    Array::iterator Array::find(const value_type &value)
    {
        pointer p = begin_;
        for(; p != end_ && *p != value; ++p)
        {}
        return iterator(this, p - begin_);
    }
   
    Array::const_iterator Array::find(const value_type &value) const
    {
        pointer p = begin_;
        for(; p != end_ && *p != value; ++p)
        {}
        return const_iterator(this, p - begin_); 
    }
    
    Object* Array::clone() const
    {
        Array *p = allocator_->createArray();
        p->reserve(this->size());
        
        for(pointer it = begin_; it != end_; ++it)
        {
            new (p->end_++) value_type(*it);
        }
        return p;
    }
    
    Object* Array::deepClone() const
    {
        Array *p = allocator_->createArray();
        p->reserve(this->size());
        
        for(pointer it = begin_; it != end_; ++it)
        {
            Node tmp = it->deepClone();
            new (p->end_++) value_type(tmp);
        }
        return p;
    }
    
    bool Array::equal(const Array *p) const
    {
        if(this->size() != p->size())
        {
            return false;
        }
        for(pointer it = begin_, it2 = p->begin_; it != end_; ++it, ++it2)
        {
            if(*it != *it2)
            {
                return false;
            }
        }
        return true;
    }
}
