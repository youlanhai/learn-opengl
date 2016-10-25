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

    void bind();
    void unbind();
    
private:
    void bindAttributes();
    void destroy();
    
    uint32_t    handle_;
    SmartPointer<ShaderProgram> shader_;
    SmartPointer<VertexBuffer>  vb_;
    SmartPointer<VertexDeclaration> decl_;
};

typedef SmartPointer<VertexAttribute> VertexAttributePtr;

#endif //VERTEX_ATTRIBUTE_H
