#ifndef SMART_POINTER_H
#define SMART_POINTER_H

#include <cassert>

template<typename T>
class SmartPointer
{
public:
    typedef T       value_type;
    typedef T*      pointer;
    typedef T&      reference;
    typedef const T* const_pointer;
    typedef const T& const_reference;
    typedef SmartPointer<T> this_type;

    SmartPointer()
        : p_(nullptr)
    {}

    SmartPointer(pointer p)
        : p_(p)
    {
        if (p_ != nullptr)
        {
            p_->retain();
        }
    }

    SmartPointer(const this_type & o)
        : p_(o.p_)
    {
        if (p_ != nullptr)
        {
            p_->retain();
        }
    }

    template<typename U>
    SmartPointer(U * p)
        : p_(nullptr)
    {
        *this = p;
    }
    
    template<typename U>
    SmartPointer(const SmartPointer<U> & o)
        : p_(nullptr)
    {
        *this = o.get();
    }
    

    ~SmartPointer()
    {
        if(p_ != nullptr)
        {
            p_->release();
        }
    }

    pointer get() const
    {
        return p_;
    }

    template<typename U>
    U* cast() const
    {
        return (U*) p_;
    }

    bool exists() const
    {
	    return p_ != 0;
    }

public:

    operator bool () const
    {
        return p_ != nullptr;
    }

    pointer operator -> () const
    {
        assert(p_ != nullptr);
        return p_;
    }

    reference operator * () const
    {
        assert(p_ != nullptr);
        return *p_;
    }

    const this_type & operator = (pointer p)
    {
        if(p_ != p)
        {
            if(p_ != nullptr)
            {
                p_->release();
            }
            p_ = p;
            if (p_ != nullptr)
            {
                p_->retain();
            }
        }
        return *this;
    }

    template<typename U>
    const this_type & operator = (U *p)
    {
        if(p_ != p)
        {
            if(p_ != nullptr)
            {
                p_->release();
                p_ = nullptr;
            }

            if(p != nullptr)
            {
#ifdef DEBUG
                p_ = dynamic_cast<pointer>(p);
                assert(p_);
#else
                p_ = pointer(p);
#endif
                p_->retain();
            }
        }
        return *this;
    }

    const this_type & operator = (const this_type & o)
    {
        return *this = o.p_;
    }

    template<typename U>
    const this_type & operator = (const SmartPointer<U> & o)
    {
        return *this = o.get();
    }

    bool operator == (const this_type &o) const { return p_ == o.p_; }
    bool operator != (const this_type &o) const { return p_ != o.p_; }

    bool operator > (const this_type &o) const { return p_ > o.p_; }
    bool operator >= (const this_type &o) const { return p_ >= o.p_; }

    bool operator < (const this_type &o) const { return p_ < o.p_; }
    bool operator <= (const this_type &o) const { return p_ <= o.p_; }

private:
    pointer p_;
};


template<typename T, typename U>
bool operator == (const SmartPointer<T> & l, const U * r)
{
    return l.get() == r;
}

template<typename T, typename U>
bool operator == (const T *l, const SmartPointer<U> & r)
{
    return l == r.get();
}

template<typename T, typename U>
bool operator == (const SmartPointer<T> & l, const SmartPointer<U> & r)
{
    return l.get() == r.get();
}

template<typename T, typename U>
bool operator != (const SmartPointer<T> & l, const U * r)
{
    return l.get() != r;
}

template<typename T, typename U>
bool operator != (const T *l, const SmartPointer<U> & r)
{
    return l != r.get();
}

template<typename T, typename U>
bool operator != (const SmartPointer<T> & l, const SmartPointer<U> & r)
{
    return l.get() != r.get();
}

#endif //SMART_POINTER_H
