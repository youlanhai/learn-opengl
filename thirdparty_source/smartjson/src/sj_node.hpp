#ifndef SMARTJSON_NODE_HPP
#define SMARTJSON_NODE_HPP

#include "sj_types.hpp"
#include "sj_iterator.hpp"

#if JSON_SUPPORT_STL_STRING
#include <string>
#endif

namespace mjson
{
    class Object;
    class String;
    class Dict;
    class Array;
    class IAllocator;
    
    class Node;
    struct NodePair;

    typedef Iterator<Array, Node>               ArrayIterator;
    typedef Iterator<const Array, const Node>   ConstArrayIterator;

    typedef Iterator<Dict, NodePair>            DictIterator;
    typedef Iterator<const Dict, const NodePair> ConstDictIterator;


    class Node
    {
    public:
        Node();
        Node(bool value);
        Node(int value);
        Node(unsigned int value);
        Node(int64_t value);
        Node(uint64_t value);
        Node(float value);
        Node(double value);
        Node(Object *p);
        Node(const char *str, size_t size = 0, IAllocator *allocator = 0);
        Node(const Node &other);
        ~Node();

        void        safeRelease();
        
        bool        isNull() const;
        bool        isBool() const;
        bool        isInt() const;
        bool        isFloat() const;
        bool        isString() const;
        bool        isArray() const;
        bool        isDict() const;
        bool        isNumber() const;
        bool        isPointer() const;
        
        bool        asBool()    const;
        int         asInt()     const;
        unsigned int asUint()   const;
        int64_t     asInt64()   const;
        uint64_t    asUint64()  const;
        Integer     asInteger() const;
        Float       asFloat()   const;
        String*     asString()  const;
        const char* asCString() const;
        Array*      asArray()   const;
        Dict*       asDict()    const;
        
        //{ NOTICE the `raw*` and `ref*` method was not safe.
        bool        rawBool() const;
        int         rawInt() const;
        int64_t     rawInt64() const;
        Integer     rawInteger() const;
        Float       rawFloat() const;
        const char* rawCString() const;


        String*     rawString() const;
        Array*      rawArray() const;
        Dict*       rawDict() const;
        
        String*     rawString();
        Array*      rawArray();
        Dict*       rawDict();
        
        const String&   refString() const;
        const Array&    refArray() const;
        const Dict&     refDict() const; 
        
        String&     refString();
        Array&      refArray();
        Dict&       refDict();

        //}


        void        setNull();
        void        setBool(bool v);
        void        setInt(Integer v);
        void        setFloat(Float v);
        void        setString(const char *str, size_t size = 0, IAllocator *allocator = 0);
        Array*      setArray(IAllocator *allocator = 0);
        Dict*       setDict(IAllocator *allocator = 0);
        
        const Node& operator = (bool value);
        const Node& operator = (int value);
        const Node& operator = (unsigned int value);
        const Node& operator = (int64_t value);
        const Node& operator = (uint64_t value);
        const Node& operator = (float value);
        const Node& operator = (double value);
        const Node& operator = (const char *value);
        const Node& operator = (const Object *value);
        const Node& operator = (const Node &value);
        
        bool operator == (const Node &value) const;
        bool operator != (const Node &value) const;
        
        size_t size() const;
        size_t capacity() const;
        void reserve(size_t capacity);
        void clear();

        Node clone() const;
        Node deepClone() const;
        
    public:
        // array
        ArrayIterator begin();
        ConstArrayIterator begin() const;

        ArrayIterator end();
        ConstArrayIterator end() const;

        void resize(size_t size);

        Node& front();
        const Node& front() const;

        Node& back();
        const Node& back() const;

        void pushBack(const Node &node);
        void popBack();
        
        ArrayIterator find(const Node &node);
        ConstArrayIterator find(const Node &node) const;

        void insert(ArrayIterator it, const Node &node);
        void erase(ArrayIterator it);
        void remove(const Node &node);

        /** index array, or get member from dict.
         *  @param index  index of array, or key of dict.
         *  @return return value if the index is in range of array or 
         *  key is in dict, otherwise null value will be returned.
         */
        Node& operator[] (SizeType index);
        const Node& operator[] (SizeType index) const;

    public:
        // dict
        DictIterator memberBegin();
        ConstDictIterator memberBegin() const;

        DictIterator memberEnd();
        ConstDictIterator memberEnd() const;

        DictIterator findMember(const char *key);
        DictIterator findMember(const Node &key);

        ConstDictIterator findMember(const char *key) const;
        ConstDictIterator findMember(const Node &key) const;

        bool hasMember(const char *key) const;
        bool hasMember(const Node &key) const;

        void setMember(const char *key, const Node &val);
        void setMember(const Node &key, const Node &val);

        void eraseMember(DictIterator it);

        void removeMember(const char *key);
        void removeMember(const Node &key);
        
        // when the key was not found, null value will be returned.
        const Node& operator[] (const char *key) const;
        const Node& operator[] (const Node &key) const;

    public:
        // std::string

#if JSON_SUPPORT_STL_STRING
        Node(const std::string &value, IAllocator *allocator = 0);
        const Node& operator = (const std::string &value);

        void setStdString(const std::string &value, IAllocator *allocator = 0);
        void asStdString(std::string &out) const;
        std::string asStdString() const;

        const Node& operator[] (const std::string &key) const;

        DictIterator findMember(const std::string &key);
        ConstDictIterator findMember(const std::string &key) const;
        bool hasMember(const std::string &key) const;
        void setMember(const std::string &key, const Node &val);
        void removeMember(const std::string &key);
#endif

    private:
        struct Value
        {
            union
            {
                bool        b;
                Integer     i;
                Float       f;
                Object*     p;
                String*     ps;
                Array*      pa;
                Dict*       pd;
            };
        };
        
        Value       value_;
        int         type_;
    };
    
    
    struct NodePair
    {
        Node        key;
        Node        value;
    };
}

#if JSON_CODE_INLINE
#include "sj_node.ipp"
#endif

#endif /* SMARTJSON_NODE_HPP */
