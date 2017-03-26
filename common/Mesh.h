#ifndef H__MESH_H
#define H__MESH_H

#include "VertexBuffer.h"
#include "VertexDeclaration.h"
#include "VertexAttribute.h"
#include "Material.h"
#include "Component.h"
#include "AABB.h"

#include <vector>

class Mesh;
typedef SmartPointer<Mesh> MeshPtr;

class MeshFaceVisitor;

class SubMesh : public ReferenceCount
{
public:
    SubMesh();
    ~SubMesh();

    void draw(Renderer *renderer);

    int getMaterialID() const { return mtlID_; }

    /** 设置图元数据
     *  useIndex表示是否使用索引缓冲区绘制。
     *  useIndex为true，则start表示索引buffer起始位置，count表示索引的个数。
     *  useIndex为false，则start表示顶点buffer起始位置，count表示顶点个数。
    */
    void setPrimitive(PrimitiveType pt,
        uint32_t start, uint32_t count, int mtlID,
        bool useIndex = true);

public:
    uint32_t        start_;
    uint32_t        count_;
    PrimitiveType   primitiveType_;
    int             mtlID_;
    bool            useIndex_;
};
typedef SmartPointer<SubMesh> SubMeshPtr;


class Mesh : public Component
{
public:
    typedef std::vector<MaterialPtr> Materials;
    typedef std::vector<SubMeshPtr> SubMeshes;

    Mesh();
    ~Mesh();

    bool load(const std::string & resource);
    
    MeshPtr clone();
    MeshPtr getSource() const { return source_; }

    virtual void draw(Renderer *renderer) override;

    void setVertexBuffer(VertexBufferPtr vertex);
    void setIndexBuffer(IndexBufferPtr index);
    void setVertexDecl(VertexDeclarationPtr decl);
	void setVertexDecl(const std::string &type);

    const SubMeshes & getSubMeshes() const;
    void setSubMeshes(const SubMeshes & subMeshes);
    void clearSubMeshes();
    void addSubMesh(SubMeshPtr subMesh);
    size_t getNbSubMesh() const;
    SubMeshPtr getSubMesh(size_t i) const;

    MaterialPtr getMaterial(int mtlID) const;
    const Materials& getMaterials() const { return materials_; }
    Materials& getMaterials() { return materials_; }
    void setMaterials(const Materials & materials);
	void setMaterial(size_t index, MaterialPtr mtl);
    int addMaterial(MaterialPtr mtl);

    const std::string& getResource() const { return resource_; }
    void setResource(const std::string & name) { resource_ = name; }

    VertexBufferPtr getVertexBuffer() const { return vertexBuffer_; }
    VertexDeclarationPtr getVertexDecl() const { return vertexDecl_; }
    IndexBufferPtr getIndexBuffer() const { return indexBuffer_; }

    void generateBoundingBox();
    const AABB& getBoundingBox() const { return boundingBox_; }

    void iterateFaces(MeshFaceVisitor &visitor) const;

private:
    MeshPtr                 source_;
    std::string             resource_;

    VertexBufferPtr         vertexBuffer_;
    IndexBufferPtr          indexBuffer_;
    VertexDeclarationPtr    vertexDecl_;
    VertexAttributePtr      vertexAttribute_;

    SubMeshes               subMeshs_;
    Materials               materials_;
    AABB                    boundingBox_;
};

#endif //H__MESH_H
