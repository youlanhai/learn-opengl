#pragma once

class Mesh;
class SubMesh;
class AABB;
class Ray;

class MeshFaceVisitor
{
public:
    /** 返回false表示停止迭代。*/
    virtual bool visit(const SubMesh *pSubMesh, const char **triangle) = 0;
};


class MeshBoundingBoxVisitor : public MeshFaceVisitor
{
    AABB & boundingBox_;
public:
    MeshBoundingBoxVisitor(AABB &bb);

    virtual bool visit(const SubMesh *pSubMesh, const char **triangle) override;
};

class MeshRayVisitor : public MeshFaceVisitor
{
    const Ray &ray_;
public:
    const char *triangle_[3];
    float   t_;
    float   u_;
    float   v_;
    bool    intersected_;

public:
    explicit MeshRayVisitor(const Ray &ray);

    virtual bool visit(const SubMesh *pSubMesh, const char **triangle) override;
};
