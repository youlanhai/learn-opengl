#include "sj_object.hpp"
#include "sj_array.hpp"
#include "sj_dict.hpp"
#include "sj_string.hpp"
#include "sj_allocator.hpp"

namespace mjson
{

#define JSON_UINT_TO_INTEGER(V) static_cast<Integer>(static_cast<UInteger>(V))
#define JSON_INTEGER_TO_UINT(V, T) static_cast<T>(static_cast<UInteger>(V))
    
    extern Node s_null;
    inline Node& nullValue()
    {
        s_null.setNull();
        return s_null;
    }

    JSON_INLINE Node::Node()
    : type_(T_NULL)
    {
        value_.p = 0;
    }
    
    JSON_INLINE Node::~Node()
    {
        safeRelease();
    }
    
    JSON_INLINE Node::Node(bool value)
    : type_(T_BOOL)
    {
        value_.b = value;
    }

    JSON_INLINE Node::Node(int value)
    : type_(T_INT)
    {
        value_.i = static_cast<Integer>(value);
    }

    JSON_INLINE Node::Node(unsigned int value)
    : type_(T_INT)
    {
        value_.i = JSON_UINT_TO_INTEGER(value);
    }

    JSON_INLINE Node::Node(int64_t value)
    : type_(T_INT)
    {
        value_.i = static_cast<Integer>(value);
    }

    JSON_INLINE Node::Node(uint64_t value)
    : type_(T_INT)
    {
        value_.i = JSON_UINT_TO_INTEGER(value);
    }

    JSON_INLINE Node::Node(float value)
    : type_(T_FLOAT)
    {
        value_.f = (Float)value;
    }
    
    JSON_INLINE Node::Node(double value)
    : type_(T_FLOAT)
    {
        value_.f = (Float)value;
    }
    
    JSON_INLINE Node::Node(Object *p)
    : type_(p ? p->type() : T_NULL)
    {
        if(p != 0)
        {
            p->retain();
        }
        value_.p = p;
    }
    
    JSON_INLINE Node::Node(const char *str, size_t size, IAllocator *allocator)
    : type_(T_NULL)
    {
        setString(str, size, allocator);
    }
    
    JSON_INLINE Node::Node(const Node &other)
    : value_(other.value_)
    , type_(other.type_)
    {
        if(isPointer())
        {
            value_.p->retain();
        }
    }
    
    /////////////////////////////////////////////////////////////
    /// types
    /////////////////////////////////////////////////////////////
    
    JSON_INLINE bool Node::isNull() const
    {
        return type_ == T_NULL;
    }
    
    JSON_INLINE bool Node::isBool() const
    {
        return type_ == T_BOOL;
    }
    
    JSON_INLINE bool Node::isInt() const
    {
        return type_ == T_INT;
    }
    
    JSON_INLINE bool Node::isFloat() const
    {
        return type_ == T_FLOAT;
    }
    
    JSON_INLINE bool Node::isString() const
    {
        return type_ == T_STRING;
    }
    
    JSON_INLINE bool Node::isArray() const
    {
        return type_ == T_ARRAY;
    }
    
    JSON_INLINE bool Node::isDict() const
    {
        return type_ == T_DICT;
    }
    
    JSON_INLINE bool Node::isNumber() const
    {
        return type_ == T_INT || type_ == T_FLOAT;
    }

    JSON_INLINE bool Node::isPointer() const
    {
        return type_ > T_POINTER;
    }
 
    /////////////////////////////////////////////////////////////
    /// conver value to json
    /////////////////////////////////////////////////////////////
    JSON_INLINE void Node::safeRelease()
    {
        if(isPointer())
        {
            value_.p->release();
        }
    }

    JSON_INLINE void Node::setNull()
    {
        safeRelease();
        type_ = T_NULL;
        value_.p = 0;
    }

    JSON_INLINE void Node::setBool(bool v)
    {
        safeRelease();
        type_ = T_BOOL;
        value_.b = v;
    }

    JSON_INLINE void Node::setInt(Integer v)
    {
        safeRelease();
        type_ = T_INT;
        value_.i = v;
    }

    JSON_INLINE void Node::setFloat(Float v)
    {
        safeRelease();
        type_ = T_FLOAT;
        value_.f = v;
    }
    

    JSON_INLINE const Node& Node::operator = (bool value)
    {
        safeRelease();
        type_ = T_BOOL;
        value_.b = value;
        return *this;
    }
    
    JSON_INLINE const Node& Node::operator = (int value)
    {
        safeRelease();
        type_ = T_INT;
        value_.i = static_cast<Integer>(value);
        return *this;
    }

    JSON_INLINE const Node& Node::operator = (unsigned int value)
    {
        safeRelease();
        type_ = T_INT;
        value_.i = JSON_UINT_TO_INTEGER(value);
        return *this;
    }

    JSON_INLINE const Node& Node::operator = (int64_t value)
    {
        safeRelease();
        type_ = T_INT;
        value_.i = static_cast<Integer>(value);
        return *this;
    }

    JSON_INLINE const Node& Node::operator = (uint64_t value)
    {
        safeRelease();
        type_ = T_INT;
        value_.i = JSON_UINT_TO_INTEGER(value);
        return *this;
    }

    JSON_INLINE const Node& Node::operator = (float value)
    {
        safeRelease();
        type_ = T_FLOAT;
        value_.f = (Float)value;
        return *this;
    }

    JSON_INLINE const Node& Node::operator = (double value)
    {
        safeRelease();
        type_ = T_FLOAT;
        value_.f = (Float)value;
        return *this;
    }
    
    JSON_INLINE const Node& Node::operator = (const char *value)
    {
        if(isPointer() && value_.p)
        {
            setString(value, 0, value_.p->getAllocator());
        }
        else
        {
            setString(value);
        }
        return *this;
    }
    
    JSON_INLINE const Node& Node::operator = (const Object *value)
    {
        if(!isPointer() || value_.p != value)
        {
            setNull();
            if(value != 0)
            {
                type_ = value->type();
                value_.p = const_cast<Object*>(value);
                value_.p->retain();
            }
        }
        return *this;
    }
    
    JSON_INLINE const Node& Node::operator = (const Node &value)
    {
        if(value.isPointer())
        {
            return *this = value.value_.p;
        }
        else
        {
            safeRelease();
            type_ = value.type_;
            value_ = value.value_;
            return *this;
        }
    }
    
    /////////////////////////////////////////////////////////////
    /// convert json to value safely
    /////////////////////////////////////////////////////////////
    
    JSON_INLINE bool Node::asBool() const
    {
        return isBool() ? value_.b : false;
    }
    
    JSON_INLINE Integer Node::asInteger() const
    {
        if(isInt()) return value_.i;
        if(isFloat()) return static_cast<Integer>(value_.f);
        return 0;
    }

    JSON_INLINE int Node::asInt() const
    {
        return static_cast<int>(asInteger());
    }

    JSON_INLINE unsigned int Node::asUint() const
    {
        return JSON_INTEGER_TO_UINT(asInteger(), unsigned int);
    }

    JSON_INLINE int64_t Node::asInt64() const
    {
        return static_cast<int64_t>(asInteger());
    }

    JSON_INLINE uint64_t Node::asUint64() const
    {
        return JSON_INTEGER_TO_UINT(asInteger(), uint64_t);
    }

    JSON_INLINE Float Node::asFloat() const
    {
        if(isFloat()) return value_.f;
        if(isInt()) return (Float)value_.i;
        return (Float)0;
    }
    
    JSON_INLINE const char* Node::asCString() const
    {
        return isString() ? rawString()->data() : "";
    }

    JSON_INLINE String* Node::asString() const
    {
        return isString() ? value_.ps : 0;
    }

    JSON_INLINE Array* Node::asArray() const
    {
        return isArray() ? value_.pa : 0;
    }

    JSON_INLINE Dict* Node::asDict() const
    {
        return isDict() ? value_.pd : 0;
    }

    /////////////////////////////////////////////////////////////
    /// convert json to value unsafe
    /////////////////////////////////////////////////////////////
    JSON_INLINE bool Node::rawBool() const
    {
        return value_.b;
    }

    JSON_INLINE int Node::rawInt() const
    {
        return (int)value_.i;
    }

    JSON_INLINE int64_t Node::rawInt64() const
    {
        return (int64_t)value_.i;
    }

    JSON_INLINE Integer Node::rawInteger() const
    {
        return value_.i;
    }

    JSON_INLINE Float Node::rawFloat() const
    {
        return value_.f;
    }

    JSON_INLINE const char* Node::rawCString() const
    {
        JSON_ASSERT(isString());
        return rawString()->data();
    }
    
    
    JSON_INLINE String* Node::rawString() const
    {
        JSON_ASSERT(isString());
        return value_.ps;
    }
    
    JSON_INLINE Array* Node::rawArray() const
    {
        JSON_ASSERT(isArray());
        return value_.pa;
    }
    
    JSON_INLINE Dict* Node::rawDict() const
    {
        JSON_ASSERT(isDict());
        return value_.pd;
    }
    


    JSON_INLINE String* Node::rawString()
    {
        JSON_ASSERT(isString());
        return value_.ps;
    }

    JSON_INLINE Array* Node::rawArray()
    {
        JSON_ASSERT(isArray());
        return value_.pa;
    }

    JSON_INLINE Dict* Node::rawDict()
    {
        JSON_ASSERT(isDict());
        return value_.pd;
    }


    JSON_INLINE String& Node::refString()
    {
        JSON_ASSERT(isString());
        return *(value_.ps);
    }

    JSON_INLINE Array& Node::refArray()
    {
        JSON_ASSERT(isArray());
        return *(value_.pa);
    }

    JSON_INLINE Dict& Node::refDict()
    {
        JSON_ASSERT(isDict());
        return *(value_.pd);
    }


    JSON_INLINE const String& Node::refString() const
    {
        JSON_ASSERT(isString());
        return *(value_.ps);
    }

    JSON_INLINE const Array& Node::refArray() const
    {
        JSON_ASSERT(isArray());
        return *(value_.pa);
    }

    JSON_INLINE const Dict& Node::refDict() const
    {
        JSON_ASSERT(isDict());
        return *(value_.pd);
    }

    /////////////////////////////////////////////////////////////
    /// 
    /////////////////////////////////////////////////////////////
    JSON_INLINE size_t Node::size() const
    {
        if(isArray())
        {
            return value_.pa->size();
        }
        else if(isDict())
        {
            return value_.pd->size();
        }
        else if(isString())
        {
            return value_.ps->size();
        }
        return 0;
    }

    JSON_INLINE size_t Node::capacity() const
    {
        if(isArray())
        {
            return value_.pa->capacity();
        }
        else if(isDict())
        {
            return value_.pd->capacity();
        }
        return 0;
    }
    
    JSON_INLINE void Node::reserve(size_t capacity)
    {
        if(isArray())
        {
            value_.pa->reserve(capacity);
        }
        else if(isDict())
        {
            value_.pd->reserve(capacity);
        }
    }

    JSON_INLINE void Node::clear()
    {
        if(isArray())
        {
            value_.pa->clear();
        }
        else if(isDict())
        {
            value_.pd->clear();
        }
    }

    JSON_INLINE bool Node::operator != (const Node &value) const
    {
        return !(*this == value);
    }
    
    JSON_INLINE Node Node::clone() const
    {
        Node ret;
        
        ret.type_ = type_;
        ret.value_ = value_;
        if(isPointer())
        {
            ret.value_.p = value_.p->clone();
            ret.value_.p->retain();
        }
        return ret;
    }
    
    JSON_INLINE Node Node::deepClone() const
    {
        Node ret;
        
        ret.type_ = type_;
        ret.value_ = value_;
        if(isPointer())
        {
            ret.value_.p = value_.p->deepClone();
            ret.value_.p->retain();
        }
        return  ret;
    }
    
    JSON_INLINE const Node& Node::operator[] (const char *key) const
    {
        if(isDict())
        {
            ConstDictIterator it = findMember(key);
            if(it != ((const Dict*)value_.pd)->end())
            {
                return it->value;
            }
        }
        return nullValue();
    }

    JSON_INLINE const Node& Node::operator[] (const Node &key) const
    {
        if(isDict())
        {
            ConstDictIterator it = findMember(key);
            if(it != ((const Dict*)value_.pd)->end())
            {
                return it->value;
            }
        }
        return nullValue();
    }

    /////////////////////////////////////////////////////////////
    /// array
    /////////////////////////////////////////////////////////////
    JSON_INLINE ArrayIterator Node::begin()
    {
        JSON_ASSERT(isArray());
        return value_.pa->begin();
    }

    JSON_INLINE ConstArrayIterator Node::begin() const
    {
        JSON_ASSERT(isArray());
        return ((const Array*)value_.pa)->begin();
    }

    JSON_INLINE ArrayIterator Node::end()
    {
        JSON_ASSERT(isArray());
        return value_.pa->end();
    }

    JSON_INLINE ConstArrayIterator Node::end() const
    {
        JSON_ASSERT(isArray());
        return ((const Array*)value_.pa)->end();
    }

    JSON_INLINE void Node::resize(size_t size)
    {
        JSON_ASSERT(isArray());
        value_.pa->resize(size);
    }

    JSON_INLINE Node& Node::front()
    {
        JSON_ASSERT(isArray());
        return value_.pa->front();
    }

    JSON_INLINE const Node& Node::front() const
    {
        JSON_ASSERT(isArray());
        return value_.pa->front();
    }

    JSON_INLINE Node& Node::back()
    {
        JSON_ASSERT(isArray());
        return value_.pa->back();
    }

    JSON_INLINE const Node& Node::back() const
    {
        JSON_ASSERT(isArray());
        return value_.pa->back();
    }

    JSON_INLINE void Node::pushBack(const Node &node)
    {
        JSON_ASSERT(isArray());
        value_.pa->push(node);
    }

    JSON_INLINE void Node::popBack()
    {
        JSON_ASSERT(isArray());
        value_.pa->pop();
    }
    
    JSON_INLINE ArrayIterator Node::find(const Node &node)
    {
        JSON_ASSERT(isArray());
        return value_.pa->find(node);
    }

    JSON_INLINE ConstArrayIterator Node::find(const Node &node) const
    {
        JSON_ASSERT(isArray());
        return ((const Array*)(value_.pa))->find(node);
    }

    JSON_INLINE void Node::insert(ArrayIterator it, const Node &node)
    {
        JSON_ASSERT(isArray());
        value_.pa->insert(it, node);
    }

    JSON_INLINE void Node::erase(ArrayIterator it)
    {
        JSON_ASSERT(isArray());
        value_.pa->erase(it);
    }

    JSON_INLINE void Node::remove(const Node &node)
    {
        JSON_ASSERT(isArray());
        value_.pa->remove(node);
    }

    JSON_INLINE const Node& Node::operator[] (SizeType index) const
    {
        return const_cast<Node*>(this)->operator[](index);
    }

    /////////////////////////////////////////////////////////////
    /// dict
    /////////////////////////////////////////////////////////////
    JSON_INLINE DictIterator Node::memberBegin()
    {
        JSON_ASSERT(isDict());
        return value_.pd->begin();
    }

    JSON_INLINE ConstDictIterator Node::memberBegin() const
    {
        JSON_ASSERT(isDict());
        return ((const Dict*)value_.pd)->begin();
    }

    JSON_INLINE DictIterator Node::memberEnd()
    {
        JSON_ASSERT(isDict());
        return value_.pd->end();
    }

    JSON_INLINE ConstDictIterator Node::memberEnd() const
    {
        JSON_ASSERT(isDict());
        return ((const Dict*)value_.pd)->end();
    }

    JSON_INLINE DictIterator Node::findMember(const char *key)
    {
        JSON_ASSERT(isDict());
        String *s = value_.pd->getAllocator()->createString(key, strlen(key), BT_NOT_CARE);
        return value_.pd->find(Node(s));
    }

    JSON_INLINE DictIterator Node::findMember(const Node &key)
    {
        JSON_ASSERT(isDict());
        return value_.pd->find(key);
    }

    JSON_INLINE ConstDictIterator Node::findMember(const char *key) const
    {
        JSON_ASSERT(isDict());
        String *s = value_.pd->getAllocator()->createString(key, strlen(key), BT_NOT_CARE);
        return ((const Dict*)value_.pd)->find(Node(s));
    }

    JSON_INLINE ConstDictIterator Node::findMember(const Node &key) const
    {
        JSON_ASSERT(isDict());
        return ((const Dict*)value_.pd)->find(key);
    }

    JSON_INLINE bool Node::hasMember(const char *key) const
    {
        return findMember(key) != ((const Dict*)value_.pd)->end();
    }

    JSON_INLINE bool Node::hasMember(const Node &key) const
    {
        return findMember(key) != ((const Dict*)value_.pd)->end();
    }

    JSON_INLINE void Node::setMember(const char *key, const Node &val)
    {
        JSON_ASSERT(isDict());
        String *s = value_.pd->getAllocator()->createString(key, strlen(key), BT_MAKE_COPY);
        value_.pd->insert(Node(s), val);
    }

    JSON_INLINE void Node::setMember(const Node &key, const Node &val)
    {
        JSON_ASSERT(isDict());
        value_.pd->insert(key, val);
    }

    JSON_INLINE void Node::eraseMember(DictIterator it)
    {
        JSON_ASSERT(isDict());
        value_.pd->erase(it);
    }

    JSON_INLINE void Node::removeMember(const char *key)
    {
        JSON_ASSERT(isDict());
        String *s = value_.pd->getAllocator()->createString(key, strlen(key), BT_NOT_CARE);
        value_.pd->remove(Node(s));
    }

    JSON_INLINE void Node::removeMember(const Node &key)
    {
        JSON_ASSERT(isDict());
        value_.pd->remove(key);
    }

#if JSON_SUPPORT_STL_STRING
    /////////////////////////////////////////////////////////////
    /// std::string
    /////////////////////////////////////////////////////////////
    JSON_INLINE Node::Node(const std::string &value, IAllocator *allocator)
    : type_(T_NULL)
    {
        setString(value.c_str(), value.size(), allocator);
    }
    
    JSON_INLINE const Node& Node::operator = (const std::string &value)
    {
        setString(value.c_str(), value.size());
        return *this;
    }

    JSON_INLINE const Node& Node::operator[] (const std::string &key) const
    {
        return operator[](key.c_str());
    }
    
    JSON_INLINE void Node::setStdString(const std::string &value, IAllocator *allocator)
    {
        return setString(value.c_str(), value.size(), allocator);
    }
    
    JSON_INLINE void Node::asStdString(std::string &out) const
    {
        if(isString())
        {
            out.assign(rawString()->data(), rawString()->size());
        }
    }
    
    JSON_INLINE std::string Node::asStdString() const
    {
        if(isString())
        {
            return std::string(rawString()->data(), rawString()->size());
        }
        return "";
    }

    JSON_INLINE DictIterator Node::findMember(const std::string &key)
    {
        JSON_ASSERT(isDict());
        String *s = value_.pd->getAllocator()->createString(key.c_str(), key.size(), BT_NOT_CARE);
        return value_.pd->find(Node(s));
    }

    JSON_INLINE ConstDictIterator Node::findMember(const std::string &key) const
    {
        JSON_ASSERT(isDict());
        String *s = value_.pd->getAllocator()->createString(key.c_str(), key.size(), BT_NOT_CARE);
        return ((const Dict*)value_.pd)->find(Node(s));
    }

    JSON_INLINE bool Node::hasMember(const std::string &key) const
    {
        return findMember(key) != ((const Dict*)value_.pd)->end();
    }

    JSON_INLINE void Node::setMember(const std::string &key, const Node &val)
    {
        JSON_ASSERT(isDict());
        // copy string buffer
        String *s = value_.pd->getAllocator()->createString(key.c_str(), key.size(), BT_MAKE_COPY);
        value_.pd->insert(Node(s), val);
    }

    JSON_INLINE void Node::removeMember(const std::string &key)
    {
        JSON_ASSERT(isDict());
        String *s = value_.pd->getAllocator()->createString(key.c_str(), key.size(), BT_NOT_CARE);
        value_.pd->remove(Node(s));
    }

#endif
} // end namespace mjson
