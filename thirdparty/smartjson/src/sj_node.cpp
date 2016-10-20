#include "sj_node.hpp"

#include <cmath>

#if !JSON_CODE_INLINE
#include "node.ipp"
#endif

namespace mjson
{
    Node s_null;
    
    void Node::setString(const char *str, size_t size, IAllocator *allocator)
    {
        if(0 == size)
        {
            size = strlen(str);
        }
        
        if(0 == allocator)
        {
            allocator = isPointer() ? (IAllocator*)value_.p->getAllocator() : IAllocator::getDefaultAllocator();
        }
        allocator->retain();
        
        safeRelease();
        type_ = T_STRING;
        value_.p = allocator->createString(str, size, BT_MAKE_COPY);
        value_.p->retain();
        
        allocator->release();
    }
    
    Array* Node::setArray(IAllocator *allocator)
    {
        if(0 == allocator)
        {
            allocator = isPointer() ? (IAllocator*)value_.p->getAllocator() : IAllocator::getDefaultAllocator();
        }
        allocator->retain();
        
        safeRelease();
        type_ = T_ARRAY;
        value_.p = allocator->createArray();
        value_.p->retain();
        
        allocator->release();
        return (Array*)value_.p;
    }
    
    Dict* Node::setDict(IAllocator *allocator)
    {
        if(0 == allocator)
        {
            allocator = isPointer() ? (IAllocator*)value_.p->getAllocator() : IAllocator::getDefaultAllocator();
        }
        allocator->retain();

        safeRelease();
        type_ = T_DICT;
        value_.p = allocator->createDict();
        value_.p->retain();
        
        allocator->release();
        return (Dict*)value_.p;
    }
    
    bool Node::operator == (const Node &other) const
    {
        if(this->isNumber() && other.isNumber())
        {
            if(this->isFloat() && other.isFloat())
            {
                return fabs(value_.f - other.value_.f) < DefaultEpsilon;
            }
            else if(this->isFloat())
            {
                return fabs(value_.f - (Float)other.value_.i) < DefaultEpsilon;
            }
            else if(other.isFloat())
            {
                return fabs((Float)value_.i - other.value_.f) < DefaultEpsilon;
            }
            else
            {
                return value_.i == other.value_.i;
            }
        }
        
        if(type_ != other.type_)
        {
            return false;
        }
        
        if(type_ == T_NULL)
        {
            return true;
        }
        else if(type_ == T_BOOL)
        {
            return value_.b == other.value_.b;
        }
        else if(type_ == T_STRING)
        {
            return rawString()->compare(other.rawString()) == 0;
        }
        else if(type_ == T_ARRAY)
        {
            return rawArray()->equal(other.rawArray());
        }
        else if(type_ == T_DICT)
        {
            return rawDict()->equal(other.rawDict());
        }
        else
        {
            JSON_ASSERT(false && "shouldn't reach here.");
            return false;
        }
    }
    
    Node& Node::operator[] (SizeType index)
    {
        if(isArray())
        {
            if(index < rawArray()->size())
            {
                return (*rawArray())[index];
            }
        }
        else if(isDict())
        {
            Dict::iterator it = rawDict()->find(Node(index));
            if(it != rawDict()->end())
            {
                return it->value;
            }
        }
        
        return nullValue();
    }
}
