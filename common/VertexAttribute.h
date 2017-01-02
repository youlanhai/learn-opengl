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
	*	����������顣
	*	ע�⣬����������ֻ��������������Ϣ������buffer������buffer��Ҫ�������鼤������ֶ����
	*	�˺����Ѿ��Զ������˶���buffer������buffer��Ҫ�ⲿ�������ֶ����
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
