#ifndef SMARTJSON_ALLOCATOR_HPP
#define SMARTJSON_ALLOCATOR_HPP

#include "sj_reference.hpp"
#include "sj_types.hpp"

namespace mjson
{
    class Object;
    class String;
    class Array;
    class Dict;
    
    class IAllocator : public Reference
    {
    public:
        IAllocator();
        ~IAllocator();
        
        virtual void*   malloc(size_t size) = 0;
        virtual void    free(void *p) = 0;
        
        virtual String* createString(const char *str, size_t size, BufferType type);
        virtual Array*  createArray();
        virtual Dict*   createDict();
        
        virtual void    freeObject(Object *p);
        
        static void setDefaultAllocator(IAllocator *p);
        static IAllocator* getDefaultAllocator(){ return s_pDefault; }
        
    private:
        static IAllocator *s_pDefault;
    };
    
}


#endif /* SMARTJSON_ALLOCATOR_HPP */
