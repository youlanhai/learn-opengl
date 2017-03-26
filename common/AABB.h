#pragma once

#include "Vector3.h"

class Matrix;

class AABB
{
public:
    Vector3 min_;
    Vector3 max_;

    AABB(){}
    ~AABB(){}

    void setZero();

    void setEmpty();
    bool isValid() const;
    
    void correct();
    
    void addPoint(const Vector3 &point);
    
    void addAABB(const AABB &other);
    
    void applyMatrix(const Matrix &matrix);
};
