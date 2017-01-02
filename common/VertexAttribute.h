#ifndef VERTEX_ATTRIBUTE_H
#define VERTEX_ATTRIBUTE_H

#include "Reference.h"
#include "SmartPointer.h"
#include <cstdint>

class VertexBuffer;
class VertexDeclaration;

class VertexAttribute : public ReferenceCount
{
public:
    VertexAttribute();
    ~VertexAttribute();

	virtual bool init(VertexBuffer *vb, VertexDeclaration *decl);

	/**
	*	激活顶点属性组。
	*	注意，顶点属性组只存贮顶点属性信息，顶点buffer和索引buffer需要在属性组激活后，再手动激活。
	*	此函数已经自动激活了顶点buffer，索引buffer需要外部调用者手动激活。
	*/
    void bind();
    void unbind();
    
private:
    void bindAttributes();
	void unbindAttributes();

    void destroy();
    
    uint32_t    handle_;
    SmartPointer<VertexBuffer>  vb_;
    SmartPointer<VertexDeclaration> decl_;
};

typedef SmartPointer<VertexAttribute> VertexAttributePtr;

#endif //VERTEX_ATTRIBUTE_H
