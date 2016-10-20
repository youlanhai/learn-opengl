#ifndef SMARTJSON_ALLOCATOR_IMP_HPP
#define SMARTJSON_ALLOCATOR_IMP_HPP

#include "sj_allocator.hpp"

namespace mjson
{
    /**
     *  Allocate memory from os directly. thread safe.
     *  This is the default allocator.
     */
    class RawAllocator : public IAllocator
    {
    public:
        RawAllocator();
        ~RawAllocator();
        
        virtual void*   malloc(size_t size);
        virtual void    free(void *p);
    };
    
    /**
     *  Allocate small memorty by using memory pool.
     *  May be efficent, but not thread safe.
     *  Still in testing stage.
     */
    class MemoryPoolAllocator : public IAllocator
    {
    public:
        explicit MemoryPoolAllocator(size_t pageSize = 100 * 1024);
        ~MemoryPoolAllocator();
        
        virtual void*   malloc(size_t size);
        virtual void    free(void *p);
        
    private:
        struct MemNode
        {
            MemNode* next;
        };
        
        struct MemSlot
        {
            char*   addr;
            MemNode* freeList;
            int     allocCount;
        };
        
        MemSlot* grow();
        void clear();
        
    private:
        MemSlot*    pool_;
        size_t      size_;
        size_t      capacity_;
        
        size_t      pageSize_;
        
        // This allocator only allocate memory whitch size <= minAllocSize_.
        size_t      minAllocSize_;
    };

} // end namespace mjson

#endif //SMARTJSON_ALLOCATOR_IMP_HPP
