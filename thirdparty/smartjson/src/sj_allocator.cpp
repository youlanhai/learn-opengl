#include "sj_allocator.hpp"

#include "sj_string.hpp"
#include "sj_array.hpp"
#include "sj_dict.hpp"

#include <new>

namespace mjson
{
    IAllocator* createDefaultAllocator();

    IAllocator::IAllocator()
    {
        
    }
    
    IAllocator::~IAllocator()
    {
        
    }
    
    String* IAllocator::createString(const char *str, size_t size, BufferType type)
    {
        void *p = this->malloc(sizeof(String));
        return new (p)String(str, size, type, this);
    }
    
    Array* IAllocator::createArray()
    {
        void *p = this->malloc(sizeof(Array));
        return new (p)Array(this);
    }
    
    Dict* IAllocator::createDict()
    {
        void *p = this->malloc(sizeof(Dict));
        return new (p)Dict(this);
    }
    
    void IAllocator::freeObject(Object *p)
    {
        this->retain();
        
        p->~Object();
        this->free(p);
        
        this->release();
    }
   
    
    /*static*/ IAllocator* IAllocator::s_pDefault = createDefaultAllocator();
    
    /*static*/ void IAllocator::setDefaultAllocator(IAllocator *p)
    {
        if(p != 0)
        {
            p->retain();
        }
        
        if(s_pDefault)
        {
            s_pDefault->release();
        }
        
        s_pDefault = p;
    }
}
