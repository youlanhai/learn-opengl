#pragma once

#include "Reference.h"
#include "SmartPointer.h"
#include "RenderState.h"

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

private:
	uint32_t oldFBO_;
	uint32_t fbo_;
	TexturePtr texture_;
};

typedef SmartPointer<FrameBuffer> FrameBufferPtr;

