#ifndef VERTEX_BUFFER_H
#define VERTEX_BUFFER_H

#include "glconfig.h"
#include "Reference.h"
#include "SmartPointer.h"

enum class BufferType
{
    Vertex = GL_ARRAY_BUFFER,
    Index = GL_ELEMENT_ARRAY_BUFFER,
};

enum class BufferUsage
{
    Static = GL_STATIC_DRAW,
    Dynamic = GL_DYNAMIC_DRAW,
};

enum class IndexType
{
    Index8 = GL_UNSIGNED_BYTE,
    Index16 = GL_UNSIGNED_SHORT,
    Index32 = GL_UNSIGNED_INT
};

size_t indexType2Size(IndexType type);
IndexType size2IndexType(size_t n);

class BufferBase : public ReferenceCount
{
    BufferBase(const BufferBase &);
    const BufferBase & operator = (const BufferBase &);

public:

    BufferBase(BufferType type, BufferUsage usage, size_t stride);
    virtual ~BufferBase();

    bool isValid() const;

    size_t size() const { return size_; }
    size_t capacity() const { return capacity_; }
    size_t stride() const { return stride_; }

    size_t count() const { return size_ / stride_; }

    char* lock(bool readOnly = false);
    bool unlock();

    void resize(size_t nCount, const void *data = nullptr);
    void fill(size_t iStart, size_t nCount, const void *data);

    virtual bool bind();
    virtual void unbind();

    virtual void onDeviceClose();

private:

    void destroy();

protected:
    BufferType  type_;
    BufferUsage usage_;
    GLuint      vb_;
    size_t      stride_;
    size_t      capacity_;
    size_t      size_;
    char *      pData_;
    bool        dirty_;
};

//顶点缓冲区
class VertexBuffer : public BufferBase
{
public:

    VertexBuffer(BufferUsage usage, size_t stride, size_t nVertex, const void *data = nullptr);
    ~VertexBuffer();

    virtual bool bind() override;
    virtual void unbind();
};


template<typename TVertex>
class VertexBufferEx : public VertexBuffer
{
public:

    VertexBufferEx(BufferUsage usage, size_t nVertex, const TVertex *data = nullptr)
        : VertexBuffer(usage, sizeof(TVertex), nVertex, data)
    {

    }

    void resizeBuffer(size_t nVertex, const TVertex *data = nullptr)
    {
        resize(nVertex, data);
    }

    void fillBuffer(size_t iStart, size_t nVertex, const TVertex *data)
    {
        fill(iStart, nVertex, data);
    }

};


//索引缓冲区
class IndexBuffer : public BufferBase
{
public:
    IndexBuffer(BufferUsage usage, size_t stride, size_t nIndex, const void *data = nullptr);
    ~IndexBuffer();

    IndexType getIndexType() const;

    virtual bool bind() override;
    virtual void unbind();
};

template<typename TIndex>
class IndexBufferEx : public IndexBuffer
{
public:

    IndexBufferEx(BufferUsage usage, size_t nIndex, const TIndex *data = nullptr)
        : IndexBuffer(usage, sizeof(TIndex), nIndex, data)
    {
    }

    void resizeBuffer(size_t nIndex, const TIndex *data = nullptr)
    {
        resize(nIndex, data);
    }

    void fillBuffer(size_t iStart, size_t nIndex, TIndex *data)
    {
        fill(iStart, nIndex, data);
    }
};

typedef SmartPointer<VertexBuffer>  VertexBufferPtr;
typedef SmartPointer<IndexBuffer>   IndexBufferPtr;

#endif //VERTEX_BUFFER_H
