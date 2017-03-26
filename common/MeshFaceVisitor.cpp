#include "MeshFaceVisitor.h"
#include "AABB.h"
#include "Ray.h"
#include <float.h>

MeshBoundingBoxVisitor::MeshBoundingBoxVisitor(AABB &bb)
    : boundingBox_(bb)
{
    boundingBox_.setEmpty();
}

bool MeshBoundingBoxVisitor::visit(const SubMesh *pSubMesh, const char **triangle)
{
    for (int i = 0; i < 3; ++i)
    {
        boundingBox_.addPoint(*(Vector3*)(triangle[i]));
    }
    return true;
}

MeshRayVisitor::MeshRayVisitor(const Ray & ray)
    : ray_(ray)
    , pSubMesh_(nullptr)
    , t_(FLT_MAX)
    , u_(0.0f)
    , v_(0.0f)
    , intersected_(false)
{
    for (int i = 0; i < 3; ++i)
    {
        triangle_[i] = nullptr;
    }
}

bool MeshRayVisitor::visit(const SubMesh * pSubMesh, const char ** triangle)
{
    float t, u, v;
    if (ray_.intersectTriangle(
        *(Vector3*)triangle[0],
        *(Vector3*)triangle[1],
        *(Vector3*)triangle[2],
        &t, &u, &v) && t < t_)
    {
        intersected_ = true;
        pSubMesh_ = pSubMesh;
        t_ = t;
        u_ = u;
        v_ = v;
        triangle_[0] = triangle[0];
        triangle_[1] = triangle[1];
        triangle_[2] = triangle[2];
    }
    return true;
}
