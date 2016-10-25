#include "VertexAttribute.h"
#include "glconfig.h"
#include "VertexBuffer.h"
#include "VertexDeclaration.h"
#include "ShaderProgram.h"
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

bool VertexAttribute::init(ShaderProgram *shader, VertexBuffer * vb, VertexDeclaration * decl)
{
    if(nullptr == shader || nullptr == vb || nullptr == decl)
    {
        return false;
    }
    
    if(shader == shader_ && vb == vb_ && decl == decl_)
    {
        return true;
    }
    
    destroy();
    
    shader_ = shader;
    vb_ = vb;
    decl_ = decl;
    
    if(isVAOSupported())
    {
        if (glIsVertexArray(handle_))
        {
            return false;
        }
        
        glCreateVertexArrays(1, &handle_);
        if (!glIsVertexArray(handle_))
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
        
        int location = shader_->getAttribLocation(e.usage);
        if(location >= 0)
        {
            glEnableVertexAttribArray(location);
            glVertexAttribPointer(location, e.nComponent, e.type, e.normalized, decl_->getVertexSize(), (GLvoid*)offset);
        }
        offset += e.size();
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
    vb_->unbind();
}
