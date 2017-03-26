#include "Mesh.h"
#include "Renderer.h"
#include "MeshFaceVisitor.h"

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

void SubMesh::draw(Renderer *renderer)
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

void Mesh::draw(Renderer *renderer)
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
        MaterialPtr mtl = renderer->getOverwriteMaterial();
        if (!mtl)
        {
            mtl = getMaterial(ptr->getMaterialID());
        }

        if(mtl && mtl->begin())
        {
            ptr->draw(renderer);

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

void Mesh::clearSubMeshes()
{
    subMeshs_.clear();
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

void Mesh::generateBoundingBox()
{
    iterateFaces(MeshBoundingBoxVisitor(boundingBox_));

    if (!boundingBox_.isValid())
    {
        boundingBox_.setZero();
    }
}

static uint32_t extractIndex(const char *pData, int stride, int index)
{
    const char *p = pData + index * stride;
    uint32_t ret = 0;
    switch (stride)
    {
    case 1:
        ret = *(uint8_t*)p;
        break;
    case 2:
        ret = *(uint16_t*)p;
        break;
    case 4:
        ret = *(uint32_t*)p;
        break;
    default:
        break;
    }
    return ret;
}

void Mesh::iterateFaces(MeshFaceVisitor & visitor) const
{
    const char* vertexData = vertexBuffer_->lock(true);
    size_t vertexStride = vertexBuffer_->stride();

    if (indexBuffer_)
    {
        const char* indexData = indexBuffer_->lock(true);
        bool ok = true;
        for (int i = 0; i < subMeshs_.size() && ok; ++i)
        {
            SubMesh *sub = subMeshs_[i].get();
            if (sub->primitiveType_ != PrimitiveType::TriangleList)
            {
                continue;
            }
            for (uint32_t k = 0; k < sub->count_ && ok; k += 3)
            {
                uint32_t a = extractIndex(indexData, indexBuffer_->stride(), sub->start_ + k + 0);
                uint32_t b = extractIndex(indexData, indexBuffer_->stride(), sub->start_ + k + 1);
                uint32_t c = extractIndex(indexData, indexBuffer_->stride(), sub->start_ + k + 2);
                const char* triangle[3] = {
                    vertexData + a * vertexStride,
                    vertexData + b * vertexStride,
                    vertexData + c * vertexStride,
                };
                ok = visitor.visit(sub, triangle);
            }
        }
        indexBuffer_->unlock();
    }
    else
    {
        bool ok = true;
        for (int i = 0; i < subMeshs_.size() && ok; ++i)
        {
            SubMesh *sub = subMeshs_[i].get();
            if (sub->primitiveType_ != PrimitiveType::TriangleList)
            {
                continue;
            }
            for (uint32_t k = 0; k < sub->count_ && ok; k += 3)
            {
                const char* triangle[3] = {
                    vertexData + k * vertexStride,
                    vertexData + (k + 1) * vertexStride,
                    vertexData + (k + 2) * vertexStride,
                };
                ok = visitor.visit(sub, triangle);
            }
        }
    }

    vertexBuffer_->unlock();
}
