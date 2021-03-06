#include "FrameBuffer.h"
#include "glconfig.h"
#include "Texture.h"
#include "LogTool.h"

FrameBuffer::FrameBuffer()
	: fbo_(0)
	, oldFBO_(0)
{
}


FrameBuffer::~FrameBuffer()
{
	destroy();
}

bool FrameBuffer::init(int width, int height)
{
    destroy();
    
    size_.set(width, height);
    
    glGenFramebuffers(1, &fbo_);
    return true;
}

bool FrameBuffer::initColorBuffer(int width, int height, TextureFormat format, bool hasStencilBuffer)
{
    if(!init(width, height))
    {
        return false;
    }

	texture_ = new Texture();
	if (!texture_->create(0, width, height, format, nullptr, GL_UNSIGNED_BYTE))
	{
		texture_ = nullptr;
		return false;
	}

	bind();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_->getHandle(), 0);
	unbind();
	return true;
}

bool FrameBuffer::initDepthBuffer(int width, int height, TextureFormat format, bool hasStencilBuffer)
{
	if(!init(width, height))
    {
        return false;
    }

	texture_ = new Texture();
	if (!texture_->create(0, width, height, format, nullptr, GL_FLOAT))
	{
		texture_ = nullptr;
		return false;
	}
    
    bind();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture_->getHandle(), 0);
    glDrawBuffer(0);
    glReadBuffer(0);
    unbind();
    return true;
}

void FrameBuffer::attachOnlyDepthTexture(TexturePtr tex)
{
    texture_ = tex;
    
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture_->getHandle(), 0);
	//glDrawBuffer(0);
	//glReadBuffer(0);
}

void FrameBuffer::destroy()
{
	if (glIsFramebuffer(fbo_))
	{
		glDeleteFramebuffers(1, &fbo_);
		fbo_ = 0;
		texture_ = nullptr;
	}
}

TexturePtr FrameBuffer::getTexture()
{
	return texture_;
}

void FrameBuffer::bind()
{
	GLint oldFBO = 0;
	GL_ASSERT(glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldFBO));
	oldFBO_ = oldFBO;

	glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
}

void FrameBuffer::unbind()
{
	GL_ASSERT(glBindFramebuffer(GL_FRAMEBUFFER, oldFBO_));
    oldFBO_ = 0;
}
