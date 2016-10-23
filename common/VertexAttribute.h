#ifndef VERTEX_ATTRIBUTE_H
#define VERTEX_ATTRIBUTE_H

#include "Reference.h"
#include "SmartPointer.h"
#include <string>

class VertexBuffer;
class VertexDeclaration;
class ShaderProgram;

class VertexAttribute : public ReferenceCount
{
public:
    VertexAttribute();
    ~VertexAttribute();

	virtual bool init(ShaderProgram *shader, VertexBuffer *vb, VertexDeclaration *decl);

private:
    uint32_t    handle_;
	SmartPointer<ShaderProgram>		shaderProgram_;
	SmartPointer<VertexBuffer>		vertexBuffer_;
	SmartPointer<VertexDeclaration> vertexDecl_;
};

#endif //VERTEX_ATTRIBUTE_H
