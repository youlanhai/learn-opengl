#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "Matrix.h"
#include <cstdint>

class Transform
{
public:
    enum DirtyFlag
    {
        DIRTY_MODEL = 1 << 0,
        DIRTY_VIEW = 1 << 1,
        DIRTY_PROJ = 1 << 2,

        DIRTY_MODEL_VIEW = DIRTY_MODEL | DIRTY_VIEW,
        DIRTY_ALL = DIRTY_MODEL | DIRTY_VIEW | DIRTY_PROJ,
    };

    Transform();
    virtual ~Transform();
    
    void setPosition(float x, float y, float z);
    void setPosition(const Vector3 &position);
    const Vector3& getPosition() const { return position_; }

    void translate(const Vector3 &delta);

    void setRotation(float pitch, float yaw, float roll);
    void setRotation(const Vector3 &rotation);
    const Vector3& getRotation() const { return rotation_;  }
    const Matrix& getRotationMatrix() const { return matRotation_; }

    void setScale(float x, float y, float z);
    void setScale(const Vector3 &scale);
	void setScale(float scale) { setScale(scale, scale, scale); }
    const Vector3& getScale() const { return scale_; }

    const Vector3& getRightVector() const { return matRotation_[0]; }
    const Vector3& getUpVector() const { return matRotation_[1]; }
    const Vector3& getForwardVector() const { return matRotation_[2]; }

    const Matrix& getModelMatrix() const;

    void lookAt(const Vector3& position, const Vector3 &target, const Vector3 &up);

protected:

    mutable uint32_t dirtyFlag_;
    mutable Matrix  matModel_;

    Vector3         position_;
    Vector3         rotation_;
    Vector3         scale_;
    Matrix          matRotation_;   
};

#endif //TRANSFORM_H
