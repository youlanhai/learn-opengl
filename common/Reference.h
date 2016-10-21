#ifndef REFERENCE_COUNT_H
#define REFERENCE_COUNT_H

/** 引用计数基类。可以安全的用于智能指针。*/
class ReferenceCount
{
public:
    ReferenceCount();
    virtual ~ReferenceCount();

    void retain()
    {
        ++referenceCount_;
    }

    virtual void release()
    {
        if (--referenceCount_ <= 0)
            delete this;
    }

    long getRefCount() const { return referenceCount_;}

private:
    long referenceCount_;
};

#endif //REFERENCE_COUNT_H
