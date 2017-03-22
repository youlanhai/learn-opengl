#include "VertexAttribute.h"
#include "glconfig.h"
#include "VertexBuffer.h"
#include "VertexDeclaration.h"
#include "LogTool.h"

VertexAttribute::VertexAttribute()
	: handle_(0)
{
}

VertexAttribute::~VertexAttribute()
{
    destroy();
}

void VertexAttribute::destroy()
{
    if(isVAOSupported() && glIsVertexArray(handle_))
    {
        glDeleteVertexArrays(1, &handle_);
        handle_ = 0;
    }
}

bool VertexAttribute::init(VertexBuffer * vb, VertexDeclaration * decl)
{
    if(nullptr == vb || nullptr == decl)
    {
        return false;
    }
    
    if(vb == vb_ && decl == decl_)
    {
        return true;
    }
    
    destroy();
    
    vb_ = vb;
    decl_ = decl;
    
    if(isVAOSupported())
    {
        if (glIsVertexArray(handle_))
        {
            return false;
        }
        
        glGenVertexArrays(1, &handle_);
        if (0 == handle_)
        {
            LOG_ERROR("Failed create vertex array");
            return false;
        }
        
        glBindVertexArray(handle_);
        vb_->bind();
        
        bindAttributes();
        
        glBindVertexArray(0);
        vb->unbind();
    }
    return true; 
}

void VertexAttribute::bindAttributes()
{
    ptrdiff_t offset = 0;
    for(size_t i = 0; i < decl_->getNumElement(); ++i)
    {
		const VertexElement &e = decl_->getElement(i);
		int location = int(e.usage);
		glEnableVertexAttribArray(location);
		glVertexAttribPointer(location, e.nComponent, e.type, e.normalized, decl_->getVertexSize(), (GLvoid*)offset);
		offset += e.size();
    }
}

void VertexAttribute::unbindAttributes()
{
	for (size_t i = 0; i < decl_->getNumElement(); ++i)
	{
		const VertexElement &e = decl_->getElement(i);
		int location = int(e.usage);
		glDisableVertexAttribArray(location);
	}
}

void VertexAttribute::bind()
{
    if(isVAOSupported())
    {
        glBindVertexArray(handle_);
        vb_->bind();
    }
    else
    {
        vb_->bind();
        bindAttributes();
    }
}

void VertexAttribute::unbind()
{
    if(isVAOSupported())
    {
        glBindVertexArray(0);
    }
	else
	{
		unbindAttributes();
	}
    vb_->unbind();
}
