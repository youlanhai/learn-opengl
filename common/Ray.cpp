#include "Ray.h"
#include "AABB.h"
#include "Matrix.h"
#include <float.h>

// 算法参考：http://www.cnblogs.com/graphics/archive/2010/08/09/1795348.html
bool Ray::intersectTriangle(const Vector3& a, const Vector3& b, const Vector3& c, float* t, float* u, float* v) const
{
    // E1
    Vector3 E1 = b - a;
    
    // E2
    Vector3 E2 = c - a;
    
    // P
    Vector3 P = direction_.crossProduct(E2);
    
    // determinant
    float det = E1.dotProduct(P);
    
    // keep det > 0, modify T accordingly
    Vector3 T;
    if( det >0 )
    {
        T = origin_ - a;
    }
    else
    {
        T = a - origin_;
        det = -det;
    }
    
    // If determinant is near zero, ray lies in plane of triangle
    if( det < 0.0001f )
        return false;
    
    // Calculate u and make sure u <= 1
    *u = T.dotProduct(P);
    if( *u < 0.0f || *u > det )
        return false;
    
    // Q
    Vector3 Q = T.crossProduct(E1);
    
    // Calculate v and make sure u + v <= 1
    *v = direction_.dotProduct(Q);
    if( *v < 0.0f || *u + *v > det )
        return false;
    
    // Calculate t, scale parameters, ray intersects triangle
    *t = E2.dotProduct(Q);
    if (*t < 0.0f)
        return false;
    
    float fInvDet = 1.0f / det;
    *t *= fInvDet;
    *u *= fInvDet;
    *v *= fInvDet;
    return true;
}

bool Ray::intersectPlane(const Vector3 &N, float D, float *distance) const
{
    float n1DotN2 = direction_.dotProduct(N);
    // 射线与平面平行
    if(fabs(n1DotN2) < FLT_EPSILON)
    {
        return false;
    }
    
    float d = (D - origin_.dotProduct(N)) / n1DotN2;
    if(d < 0.0)
    {
        return false;
    }
    
    if(distance) *distance = d;
    return true;
}

bool Ray::intersectTriangle(const Vector3 &a, const Vector3 &b, const Vector3 &c, float *distance) const
{
    float t, u, v;
    if(intersectTriangle(a, b, c, &t, &u, &v))
    {
        if(distance) *distance = t;
        return true;
    }
    return false;
}

bool Ray::intersectAABB(const AABB &ab, float *distance) const
{
    Vector3 base(0.0f, 0.0f, 0.0f);
    int couter = 0;

    for (int i = 0; i < 3; ++i)
    {
        if (origin_[i] <= ab.min_[i])
        {
            base[i] = ab.min_[i];
        }
        else if (origin_[i] >= ab.max_[i])
        {
            base[i] = ab.max_[i];
        }
        else //该方向在包围盒里
        {
            couter++;
            float offset_min_v = origin_[i] - ab.min_[i];
            float offset_max_v = ab.max_[i] - origin_[i];
            base[i] = (offset_min_v < offset_max_v) ? ab.min_[i] : ab.max_[i];
        }
    }

    if (couter == 3) //起点在包围盒里
    {
        if(distance) *distance = 0.0f;
        return true;
    }

    Vector3 ptOnPlane;
    for (int axis = 0; axis < 3; axis++)
    {
        if (fabs(direction_[axis]) > FLT_EPSILON)
        {
            float t = (base[axis] - origin_[axis]) / direction_[axis];
            if (t > 0.f)
            {
                ptOnPlane = origin_ + direction_ * t;

                int nextAxis = (axis + 1) % 3;
                int prevAxis = (axis + 2) % 3;

                if (ab.min_[nextAxis] < ptOnPlane[nextAxis] &&
                    ptOnPlane[nextAxis] < ab.max_[nextAxis] &&
                    ab.min_[prevAxis] < ptOnPlane[prevAxis] &&
                    ptOnPlane[prevAxis] < ab.max_[prevAxis])
                {
                    if(distance) *distance = t;
                    return true;
                }
            }
        }
    }
    return false;
}

bool Ray::intersectSphere(const Vector3 &center, float radius, float *out1, float *out2) const
{
    Vector3 ro = center - origin_;
    float projection = ro.dotProduct(direction_);
    if(projection < 0)
    {
        return false;
    }
    
    // 勾股定理求出垂线长度的平方。
    float distanceSq = ro.lengthSq() - projection * projection;
    
    float radiusSq = radius * radius;
    if(distanceSq > radiusSq)
    {
        return false;
    }
    
    if(out1 != nullptr && out2 != nullptr)
    {
        // 勾股定理求出交点到垂点距离的一半
        float distance2Sq = distanceSq - radiusSq;
        
        // 只有一个交点
        if(fabs(distance2Sq) < FLT_EPSILON)
        {
            *out1 = *out2 = projection;
        }
        else // 两个交点
        {
            float distance2 = sqrtf(distanceSq);
            *out1 = projection - distance2;
            *out2 = projection + distance2;
            
            // 小于0，表示起点就在圆内
            if(*out1 < 0.0)
            {
                *out1 = *out2;
            }
        }
    }
    return true;
}

void Ray::applyMatrix(const Matrix &m)
{
    origin_ = m.transformPoint(origin_);

    direction_ = m.transformNormal(direction_);
    direction_.normalize();
}
