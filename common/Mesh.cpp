#include "Mesh.h"

SubMesh::SubMesh()
    : start_(0)
    , count_(0)
    , mtlID_(-1)
    , useIndex_(true)
{

}

SubMesh::~SubMesh()
{

}

void SubMesh::setPrimitive(PrimitiveType pt,
    uint32_t start, uint32_t count, int mtlID,
    bool useIndex)
{
    primitiveType_ = pt;
    start_ = start;
    count_ = count;
    mtlID_ = mtlID;
    useIndex_ = useIndex;
}

void SubMesh::draw()
{
    if(count_ == 0)
    {
        return;
    }
    
    if(VertexBuffer::s_vertexBuffer == nullptr)
    {
        return;
    }

    if(useIndex_)
    {
        IndexBuffer *ib = IndexBuffer::s_indexBuffer;
        if(ib == nullptr)
        {
            return;
        }
        
        glDrawElements((GLenum)primitiveType_, count_, (GLenum)ib->getIndexType(), (GLvoid*)(start_ * ib->stride()));
    }
    else
    {
        glDrawArrays((GLenum)primitiveType_, start_, count_);
    }
}


/////////////////////////////////////////////////////////////
/// Mesh
/////////////////////////////////////////////////////////////

Mesh::Mesh()
{
    vertexAttribute_ = new VertexAttribute();
}

Mesh::~Mesh()
{
}

bool Mesh::load(const std::string & resource)
{
    resource_ = resource;
    return false;
}

MeshPtr Mesh::clone()
{
    MeshPtr mesh = new Mesh();
    mesh->source_ = this;
    mesh->resource_ = this->resource_;
    mesh->vertexBuffer_ = this->vertexBuffer_;
    mesh->indexBuffer_ = this->indexBuffer_;
    mesh->vertexDecl_ = this->vertexDecl_;
    mesh->vertexAttribute_ = this->vertexAttribute_;
    mesh->subMeshs_ = this->subMeshs_;

    mesh->materials_ = this->materials_;
    // for(auto mtl : this->materials_)
    // {
    //     mesh->materials_.push_back(mtl->clone());
    // }

    return mesh;
}

void Mesh::draw()
{
    if(!vertexBuffer_ || !vertexDecl_)
    {
        return;
    }

	MaterialPtr firstMaterial = getMaterial(0);
	if (!firstMaterial)
	{
		return;
	}
    
    if(!vertexAttribute_->init(vertexBuffer_.get(), vertexDecl_.get()))
    {
        return;
    }
    
    vertexAttribute_->bind();
	if (indexBuffer_)
		indexBuffer_->bind();

    for(SubMeshPtr ptr : subMeshs_)
    {
        MaterialPtr mtl = getMaterial(ptr->getMaterialID());
        if(mtl && mtl->begin())
        {
            ptr->draw();

			mtl->end();
        }
    }

    vertexAttribute_->unbind();
    if(indexBuffer_)
        indexBuffer_->unbind();
}

void Mesh::setVertexBuffer(VertexBufferPtr vertex)
{
    vertexBuffer_ = vertex;
}

void Mesh::setIndexBuffer(IndexBufferPtr index)
{
    indexBuffer_ = index;
}

void Mesh::setVertexDecl(VertexDeclarationPtr decl)
{
    vertexDecl_ = decl;
}

void Mesh::setVertexDecl(const std::string & type)
{
	vertexDecl_ = VertexDeclMgr::instance()->get(type);
}

void Mesh::setSubMeshes(const Mesh::SubMeshes & subMeshes)
{
    subMeshs_ = subMeshes;
}

void Mesh::addSubMesh(SubMeshPtr subMesh)
{
    subMeshs_.push_back(subMesh);
}

const Mesh::SubMeshes & Mesh::getSubMeshes() const
{
    return subMeshs_;
}

size_t Mesh::getNbSubMesh() const
{
    return subMeshs_.size();
}

SubMeshPtr Mesh::getSubMesh(size_t i) const
{
    return subMeshs_.at(i);
}

MaterialPtr Mesh::getMaterial(int mtlID) const
{
    if(mtlID < 0 || mtlID >= int(materials_.size()))
        return nullptr;

    return materials_[mtlID];
}

void Mesh::setMaterials(const Mesh::Materials & materials)
{
    materials_ = materials;
}

void Mesh::setMaterial(size_t index, MaterialPtr mtl)
{
	if (index < materials_.size())
	{
		materials_[index] = mtl;
	}
}

int Mesh::addMaterial(MaterialPtr mtl)
{
    int index = int(materials_.size());
    materials_.push_back(mtl);
    return index;
}

