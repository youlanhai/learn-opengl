#include "sj_allocator_imp.hpp"

#include "sj_string.hpp"
#include "sj_array.hpp"
#include "sj_dict.hpp"

#include <string.h>

namespace mjson
{
    IAllocator* createDefaultAllocator()
    {
        IAllocator *p = new RawAllocator();
        p->retain();
        return p;
    }

    RawAllocator::RawAllocator()
    {
        
    }

    RawAllocator::~RawAllocator()
    {
        
    }
    
    void* RawAllocator::malloc(size_t size)
    {
        return ::malloc(size);
    }
    
    void RawAllocator::free(void *p)
    {
        return ::free(p);
    }
    
    
    MemoryPoolAllocator::MemoryPoolAllocator(size_t pageSize)
    : pool_(0)
    , size_(0)
    , capacity_(0)
    , pageSize_(pageSize)
    {
        minAllocSize_ = maxSize(sizeof(String), maxSize(sizeof(Array), sizeof(Dict)));
    }
    
    MemoryPoolAllocator::~MemoryPoolAllocator()
    {
        if(pool_ != NULL)
        {
            clear();
            ::free(pool_);
        }
    }
    
    MemoryPoolAllocator::MemSlot* MemoryPoolAllocator::grow()
    {
        size_t newSize = size_ + 1;
        if(newSize > capacity_)
        {
            capacity_ = growCapacity(capacity_, newSize);
            MemSlot *old = pool_;
            
            pool_ = (MemSlot*)::malloc(capacity_ * sizeof(MemSlot));
            if(size_ != 0)
            {
                memcpy(pool_, old, size_ * sizeof(MemSlot));
            }
            
            if(old != NULL)
            {
                ::free(old);
            }
        }
        
        MemSlot *slot = pool_ + size_;
        size_ = newSize;
        
        slot->addr = (char*)::malloc(pageSize_);
        slot->allocCount = 0;
        slot->freeList = NULL;
        
        for(char *p = slot->addr; p < slot->addr + pageSize_; p += minAllocSize_)
        {
            MemNode *mp = (MemNode*)p;
            if(slot->freeList != NULL)
            {
                mp->next = slot->freeList;
                slot->freeList = mp;
            }
            else
            {
                mp->next = NULL;
                slot->freeList= mp;
            }
        }
        return slot;
    }
    
    void MemoryPoolAllocator::clear()
    {
        for(size_t i = 0; i < size_; ++i)
        {
            MemSlot &slot = pool_[i];
            ::free(slot.addr);
        }
        size_ = 0;
    }
    
    void* MemoryPoolAllocator::malloc(size_t size)
    {
        if(size > minAllocSize_)
        {
            return ::malloc(size);
        }
        
        MemSlot *slot = NULL;
        for(size_t i = 0; i < size_; ++i)
        {
            if(pool_[i].freeList != NULL)
            {
                slot = pool_ + i;
                break;
            }
        }
        
        if(NULL == slot)
        {
            slot = grow();
        }
        
        MemNode *p = slot->freeList;
        slot->freeList = p->next;
        slot->allocCount++;
        p->next = NULL;
        return p;
    }
    
    void MemoryPoolAllocator::free(void *p)
    {
        for(size_t i = 0; i < size_; ++i)
        {
            MemSlot &slot = pool_[i];
            if(p >= slot.addr && p < slot.addr + pageSize_)
            {
                MemNode *mp = (MemNode*)p;
                mp->next = slot.freeList;
                slot.freeList = mp;
                --slot.allocCount;
                
                JSON_ASSERT(slot.allocCount >= 0);
                return;
            }
        }
        
        // not allocated by this allocator
        ::free(p);
    }
}
