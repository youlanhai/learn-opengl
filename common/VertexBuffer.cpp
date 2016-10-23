#include "VertexBuffer.h"
#include "LogTool.h"

int g_vb_counter = 0;
int g_ib_counter = 0;

size_t indexType2Size(IndexType type)
{
    switch(type)
    {
    case IndexType::Index8:
        return sizeof(uint8_t);

    case IndexType::Index16:
        return sizeof(uint16_t);

    case IndexType::Index32:
        return sizeof(uint32_t);

    default:
        assert(0 && "Invalid IndexType");
        return 0;
    }
}

IndexType size2IndexType(size_t n)
{
    if(n <= sizeof(uint8_t))
        return IndexType::Index8;

    else if(n <= sizeof(uint16_t))
        return IndexType::Index16;

    else
        return IndexType::Index32;
}

//////////////////////////////////////////////////////////////////////
/// BufferBase
//////////////////////////////////////////////////////////////////////

BufferBase::BufferBase(BufferType type, BufferUsage usage, size_t stride)
    : type_(type)
    , usage_(usage)
    , vb_(0)
    , stride_(stride)
    , capacity_(0)
    , size_(0)
    , pData_(0)
    , dirty_(false)
{
}

BufferBase::~BufferBase()
{
    destroy();
}

bool BufferBase::isValid() const
{
    return vb_ != 0;
}

char * BufferBase::lock(bool readOnly /*= false*/)
{
    if(nullptr == pData_)
    {
        pData_ = new char[capacity_];
    }

    if(!readOnly)
    {
        dirty_ = true;
    }

    return pData_;
}

bool BufferBase::unlock()
{
    return true;
}

void BufferBase::resize(size_t nCount, const void *data /*= nullptr*/)
{
    size_ = stride_ * nCount;
    if (size_ > capacity_)
    {
        capacity_ = (size_ + 7) & (~7);
        
        delete [] pData_;
        pData_ = new char[capacity_];
    }
    else if(capacity_ > 128 && capacity_ > (size_ << 1))
    {
        delete [] pData_;
        pData_ = new char[capacity_];
    }

    if(data != nullptr)
    {
        memcpy(pData_, data, size_);
    }

    dirty_ = true;
}

void BufferBase::fill(size_t iStart, size_t nCount, const void *data)
{
    assert((iStart + nCount) * stride_ <= size_ && "BufferBase::fill - invalid offset and size!");
	assert(pData_ != nullptr);

    memcpy(pData_ + iStart * stride_, data, nCount * stride_);
    dirty_ = true;
}

void BufferBase::destroy()
{
    if(vb_ != 0)
    {
        GL_ASSERT( glDeleteBuffers(1, &vb_) );
        vb_ = 0;
    }

    if(pData_ != nullptr)
    {
        delete [] pData_;
        pData_ = nullptr;
    }
}

bool BufferBase::bind()
{
    if(0 == vb_)
    {
        GL_ASSERT(glGenBuffers(1, &vb_));
    }

    if(0 == vb_)
    {
        return false;
    }

    GL_ASSERT(glBindBuffer(GLenum(type_), vb_));

    if(dirty_)
    {
        dirty_ = false;
        GL_ASSERT(glBufferData(GLenum(type_), size_, pData_, GLenum(usage_)));
    }
    return true;
}

void BufferBase::unbind()
{
    GL_ASSERT( glBindBuffer(GLenum(type_), 0) );
}

void BufferBase::onDeviceClose()
{
    destroy();
}

//////////////////////////////////////////////////////////////////////
/// VertexBuffer
//////////////////////////////////////////////////////////////////////

VertexBuffer::VertexBuffer(BufferUsage usage, size_t stride, size_t nVertex, const void *data)
    : BufferBase(BufferType::Vertex, usage, stride)
{
    ++g_vb_counter;
    
    resize(nVertex, data);
}

VertexBuffer::~VertexBuffer()
{
    --g_vb_counter;
    
    //if (renderDevValid())
    //    renderDev()->unsetVertexBuffer(this);
}

bool VertexBuffer::bind()
{
    if(!BufferBase::bind())
    {
        return false;
    }

    //renderDev()->setVertexBuffer(this);
    return true;
}

void VertexBuffer::unbind()
{
    BufferBase::unbind();
    //renderDev()->unsetVertexBuffer(this);
}

//////////////////////////////////////////////////////////////////////
/// IndexBuffer
//////////////////////////////////////////////////////////////////////

IndexBuffer::IndexBuffer(BufferUsage usage, size_t stride, size_t nCount, const void *data)
    : BufferBase(BufferType::Index, usage, stride)
{
    ++g_ib_counter;
    
    resize(nCount, data);
}

IndexBuffer::~IndexBuffer()
{
    --g_ib_counter;
    
   /* if (renderDevValid())
        renderDev()->unsetIndexBuffer(this);*/
}

IndexType IndexBuffer::getIndexType() const
{
    return size2IndexType(stride_);
}

bool IndexBuffer::bind()
{
    if(!BufferBase::bind())
    {
        return false;
    }
    //renderDev()->setIndexBuffer(this);
    return true;
}

void IndexBuffer::unbind()
{
    BufferBase::unbind();
    //renderDev()->unsetIndexBuffer(this);
}
