#pragma once
#include "Vector3.h"

class AABB;

class Ray
{
public:
    Vector3 origin_;
    Vector3 direction_;

    Ray(){}
    ~Ray(){}
    
    /** 判断射线与平面相交。
     *  @param N    平面的法线
     *  @param D    平面的距离
     *  @param distance 如果射线与平面相交，返回射线起点到交点的距离。
     */
    bool intersectPlane(const Vector3 &N, float D, float *distance = nullptr) const;
    
    bool intersectTriangle(const Vector3 &a, const Vector3 &b, const Vector3 &c, float* t, float* u, float* v) const;

    bool intersectTriangle(const Vector3 &a, const Vector3 &b, const Vector3 &c, float *distance = nullptr) const;
    
    /** 判断射线与ABB相交。
     *  如果相交，并且distance不为空，distance返回射线起点到相交的距离。
     *  如果射线起点就在AABB内，相交距离返回0.
     */
    bool intersectAABB(const AABB &ab, float *distance = nullptr) const;
    
    /** 判断射线与球体相交。
     *  如果相交，out1和out2返回射线原点到相交点的距离。
     *  如果只有一个交点（相切或者射线起点在球体内），out1与out2相等。
     */
    bool intersectSphere(const Vector3 &center, float radius, float *out1 = nullptr, float *out2 = nullptr) const;
};
