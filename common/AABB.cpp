#include "AABB.h"
#include "Matrix.h"
#include <float.h>
#include <algorithm>

void AABB::setZero()
{
    min_ = Vector3::Zero;
    max_ = Vector3::Zero;
}

void AABB::setEmpty()
{
    min_.set(FLT_MAX, FLT_MAX, FLT_MAX);
    max_.set(FLT_MIN, FLT_MIN, FLT_MIN);
}

bool AABB::isValid() const
{
    return min_.x <= max_.x && min_.y <= max_.y && min_.z <= max_.z;
}

void AABB::correct()
{
    if(min_.x > max_.x) std::swap(min_.x, max_.x);
    if(min_.y > max_.y) std::swap(min_.y, max_.y);
    if(min_.z > max_.z) std::swap(min_.z, max_.z);
}

void AABB::addPoint(const Vector3 &point)
{
    if(point.x < min_.x) min_.x = point.x;
    if(point.y < min_.y) min_.y = point.y;
    if(point.z < min_.z) min_.z = point.z;
    
    if(point.x > max_.x) max_.x = point.x;
    if(point.y > max_.y) max_.y = point.y;
    if(point.z > max_.z) max_.z = point.z;
}

void AABB::addAABB(const AABB &other)
{
    addPoint(other.min_);
    addPoint(other.max_);
}

void AABB::applyMatrix(const Matrix &matrix)
{
    Vector3 points[8] = {
        {min_.x, min_.y, min_.z}, // front left bottom
        {min_.x, max_.y, min_.z}, // front left top
        {max_.x, max_.y, min_.z}, // front right top
        {max_.x, min_.y, min_.z}, // front right bottom
        
        {min_.x, min_.y, max_.z}, // back left bottom
        {min_.x, max_.y, max_.z}, // back left top
        {max_.x, max_.y, max_.z}, // back right top
        {max_.x, min_.y, max_.z}, // back right bottom
    };
    
    setEmpty();
    
    for(Vector3 &p : points)
    {
        p = matrix.transformPoint(p);
        addPoint(p);
    }
}
