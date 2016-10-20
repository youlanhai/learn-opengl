#include "sj_dict.hpp"
#include "sj_allocator.hpp"
#include "sj_string.hpp"

#if !JSON_CODE_INLINE
#include "sj_dict.ipp"
#endif

namespace mjson
{
    
    
    
    Dict::~Dict()
    {
        clear();
        if(capacity_ > 0)
        {
            allocator_->free(begin_);
        }
    }
    
    void Dict::reserve(size_t capacity)
    {
        if(capacity_ < capacity)
        {
            size_t size = this->size();
            value_type* old = begin_;
            
            begin_ = (value_type*)allocator_->malloc(capacity * sizeof(value_type));
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
    
    void Dict::_ensure(size_t n)
    {
        size_t newSize = size() + n;
        if(newSize > capacity_)
        {
            size_t newCapacity = growCapacity(capacity_, newSize);
            reserve(newCapacity);
        }
    }

    void Dict::clear()
    {
        for(pointer it = begin_; it != end_; ++it)
        {
            it->~value_type();
        }
        end_ = begin_;
    }

    Dict::iterator Dict::find(const Node &key)
    {
        pointer it = begin_;
        for(; it != end_ && it->key != key; ++it)
        {}
        
        return iterator(this, it - begin_);
    }
    
    Node& Dict::operator[] (const Node &key)
    {
        iterator it = find(key);
        if(it != end())
        {
            return it->value;
        }
        else
        {
            append(key, Node());
            return (end_ - 1)->value;
        }
    }
    
    const Node& Dict::operator[] (const Node &key) const
    {
        const_iterator it = find(key);
        if(it != end())
        {
            return it->value;
        }
        else
        {
            return nullValue();
        }
    }
    
    Dict::iterator Dict::insert(const Node &key, const Node &value)
    {
        iterator it = find(key);
        if(it != end())
        {
            it->value = value;
        }
        else
        {
            it = end();
            append(key, value);
        }
        return it;
    }

    void Dict::append(const Node &key, const Node &value)
    {
        _ensure(1);

        new (end_) value_type();
        end_->key = key;
        end_->value = value;
        ++end_;
    }
 
    void Dict::erase(iterator it)
    {
        if(it < end())
        {
            pointer p = begin_ + it.index();
            p->~value_type();
            
            pointer next = p + 1;
            if(next < end_)
            {
                memmove(p, next, (end_ - next) * sizeof(value_type));
            }
            
            --end_;
        }
    }
    
    Object* Dict::clone() const
    {
        Dict *p = allocator_->createDict();
        p->reserve(this->size());
        
        pointer out = p->begin_;
        for(pointer it = begin_; it != end_; ++it, ++out)
        {
            new (p->end_) value_type(*it);
        }
        
        p->end_ = out;
        return p;
    }
    
    Object* Dict::deepClone() const
    {
        Dict *p = allocator_->createDict();
        p->reserve(this->size());
        
        pointer out = p->begin_;
        for(pointer it = begin_; it != end_; ++it, ++out)
        {
            value_type tmp;
            tmp.key = it->key.deepClone();
            tmp.value = it->value.deepClone();
            
            new (out) value_type(tmp);
        }
        
        p->end_ = out;
        return p;
    }
    
    bool Dict::equal(const Dict *p) const
    {
        if(this->size() != p->size())
        {
            return false;
        }
        pointer it2 = p->begin_;
        for(pointer it = this->begin_; it != this->end_; ++it, ++it2)
        {
            if(it->key != it2->key || it->value != it2->value)
            {
                return false;
            }
        }
        return true;
    }
}
