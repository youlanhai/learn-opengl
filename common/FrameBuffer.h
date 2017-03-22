#pragma once

#include "Reference.h"
#include "SmartPointer.h"
#include "RenderState.h"
#include "Vector2.h"

class Texture;
typedef SmartPointer<Texture> TexturePtr;

class FrameBuffer : public ReferenceCount
{
public:
	FrameBuffer();
	~FrameBuffer();

	bool initColorBuffer(int width, int height, TextureFormat format, bool hasStencilBuffer = false);
	bool initDepthBuffer(int width, int height, TextureFormat format, bool hasStencilBuffer = false);
	void destroy();

	void bind();
	void unbind();

	TexturePtr getTexture();
    Vector2 getSize() const { return size_; }

private:
    Vector2     size_;
    uint32_t    fbo_;
    uint32_t    oldFBO_;
	TexturePtr  texture_;
};

typedef SmartPointer<FrameBuffer> FrameBufferPtr;

