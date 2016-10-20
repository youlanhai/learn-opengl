#include "sj_node.hpp"

namespace mjson
{
    JSON_INLINE Array::Array(IAllocator *allocator)
    : Object(allocator)
    , begin_(0)
    , end_(0)
    , capacity_(0)
    {
        
    }
    
    JSON_INLINE void Array::push(const value_type &value)
    {
        this->insert(this->end(), value);
    }
    
    JSON_INLINE Array::iterator Array::begin()
    {
        return iterator(this, 0);
    }
    
    JSON_INLINE Array::iterator Array::end()
    {
        return iterator(this, end_ - begin_);
    }
    
    JSON_INLINE Array::const_iterator Array::begin() const
    {
        return const_iterator(this, 0);
    }
    
    JSON_INLINE Array::const_iterator Array::end() const
    {
        return const_iterator(this, end_ - begin_);
    }
    
    JSON_INLINE const Array::value_type& Array::front() const
    {
        return const_cast<Array*>(this)->front();
    }
    
    JSON_INLINE const Array::value_type& Array::back() const
    {
        return const_cast<Array*>(this)->back();
    }
    
    JSON_INLINE Array::value_type& Array::operator[](size_t index)
    {
        return this->at(index);
    }
    
    JSON_INLINE const Array::value_type& Array::operator[](size_t index) const
    {
        return this->at(index);
    }
    
    JSON_INLINE const Array::value_type& Array::at(size_t index) const
    {
        return const_cast<Array*>(this)->at(index);
    }
    
    JSON_INLINE bool Array::empty() const
    {
        return this->size() == 0;
    }
    
    JSON_INLINE size_t Array::capacity() const
    {
        return capacity_;
    }
    
    JSON_INLINE Type Array::type() const
    {
        return T_ARRAY;
    }
    
    JSON_INLINE void Array::clear()
    {
        this->resize(0);
    }
    
    JSON_INLINE void Array::remove(const value_type &value)
    {
        erase(find(value));
    }
    
    JSON_INLINE void Array::pop()
    {
        JSON_ASSERT(!empty());
        erase(end() - 1);
    }
    
    JSON_INLINE Array::value_type& Array::at(size_t index)
    {
        JSON_ASSERT(index < size());
        return  begin_[index];
    }
    
    JSON_INLINE size_t Array::size() const
    {
        return end_ - begin_;
    }
    
    JSON_INLINE Array::value_type& Array::front()
    {
        JSON_ASSERT(!empty());
        return *begin_;
    }
    
    JSON_INLINE Array::value_type& Array::back()
    {
        JSON_ASSERT(!empty());
        return *(end_ - 1);
    }
}
