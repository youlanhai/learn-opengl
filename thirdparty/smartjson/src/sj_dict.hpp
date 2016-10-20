#ifndef SMARTJSON_DICT_HPP
#define SMARTJSON_DICT_HPP

#include "sj_object.hpp"
#include "sj_iterator.hpp"

namespace mjson
{
    class Node;
    struct NodePair;
    
    class Dict : public Object
    {
        JSON_DISABLE_COPY_ASSIGN(Dict);
    public:
        typedef NodePair                value_type;
        typedef NodePair*               pointer;
        typedef Iterator<Dict, NodePair>          iterator;
        typedef Iterator<const Dict, const NodePair>    const_iterator;
        
        explicit Dict(IAllocator *allocator);
        ~Dict();
        
        iterator begin();
        iterator end();
        
        const_iterator begin() const;
        const_iterator end() const;
        
        void remove(const Node &key);
        void erase(iterator it);

        iterator find(const Node &key);
        const_iterator find(const Node &key) const;
        
        bool exist(const Node &key) const;
        
        Node& operator[] (const Node &key);
        const Node& operator[] (const Node &key) const;
        
        // if the key exist, replace it's value. else insert the key-value pair to end.
        iterator insert(const Node &key, const Node &value);
        
        bool empty() const;
        size_t size() const;
        size_t capacity() const;
        
        void reserve(size_t size);
        void clear();
        
        bool equal(const Dict *p) const;
        
        virtual Type type() const;
        virtual Object* clone() const;
        virtual Object* deepClone() const;
        
    public:
        // internal method

        value_type& at(size_t index);
        const value_type& at(size_t key) const;
        
        // insert the key-value pair to end directly.
        void append(const Node &key, const Node &value);
        
    private:
        void _ensure(size_t n);
        
        value_type*     begin_;
        value_type*     end_;
        size_t          capacity_;
    };
}

#if JSON_CODE_INLINE
#include "sj_dict.ipp"
#endif

#endif /* SMARTJSON_DICT_HPP */
