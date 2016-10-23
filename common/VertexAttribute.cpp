#include "VertexAttribute.h"
#include "glconfig.h"
#include "VertexBuffer.h"
#include "VertexDeclaration.h"

VertexAttribute::VertexAttribute()
	: handle_(0)
{
}

VertexAttribute::~VertexAttribute()
{
}

bool VertexAttribute::init(ShaderProgram *shader, VertexBuffer * vb, VertexDeclaration * decl)
{
	if (glIsVertexArray(handle_))
	{
		return false;
	}

	glCreateVertexArrays(1, &handle_);
	if (!glIsVertexArray(handle_))
	{
		return false;
	}

	shaderProgram_ = shader;
	vertexBuffer_ = vb;
	vertexDecl_ = decl;

	glBindVertexArray(handle_);
	vertexBuffer_->bind();

	glBindVertexArray(0);
	vertexBuffer_->unbind();
	return false;
}
