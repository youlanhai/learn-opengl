#include "Transform.h"
#include "MathDef.h"
#include "Application.h"

Transform::Transform()
    : dirtyFlag_(DIRTY_ALL)
    , scale_(Vector3::One)
    , matRotation_(Matrix::Identity)
{
}

Transform::~Transform()
{
}

void Transform::setPosition(float x, float y, float z)
{
    position_.set(x, y, z);
    dirtyFlag_ |= DIRTY_MODEL_VIEW;
}

void Transform::setPosition(const Vector3 & position)
{
    position_ = position;
    dirtyFlag_ |= DIRTY_MODEL_VIEW;
}

void Transform::translate(const Vector3 & delta)
{
    setPosition(position_ + delta);
}

void Transform::setRotation(float pitch, float yaw, float roll)
{
    rotation_.set(pitch, yaw, roll);
    matRotation_.setRotate(pitch, yaw, roll);
    dirtyFlag_ |= DIRTY_MODEL_VIEW;
}

void Transform::setRotation(const Vector3 &rotation)
{
    rotation_ = rotation;
    matRotation_.setRotate(rotation.x, rotation.y, rotation.z);
    dirtyFlag_ |= DIRTY_MODEL_VIEW;
}

void Transform::setScale(float x, float y, float z)
{
    scale_.set(x, y, z);
    dirtyFlag_ |= DIRTY_MODEL;
}

void Transform::setScale(const Vector3 &scale)
{
    scale_ = scale;
    dirtyFlag_ |= DIRTY_MODEL;
}

void Transform::lookAt(const Vector3 & position, const Vector3 & target, const Vector3 & up)
{
    dirtyFlag_ |= DIRTY_MODEL_VIEW;
    position_ = position;

    Vector3 forward = target - position;
    forward.normalize();

    Vector3 right = up.crossProduct(forward);
    right.normalize();

    Vector3 newUp = forward.crossProduct(right);
    newUp.normalize();

    matRotation_.setIdentity();
    matRotation_[0] = right;
    matRotation_[1] = newUp;
    matRotation_[2] = forward;

    rotation_ = matRotation_.getRotate();
}

const Matrix& Transform::getModelMatrix() const
{
    if(dirtyFlag_ & DIRTY_MODEL)
    {
        dirtyFlag_ &= ~DIRTY_MODEL;

		Matrix matScale;
		matScale.setScale(scale_);

		matModel_.multiply(matRotation_, matScale);
        matModel_[3] = position_;
    }
    return matModel_;
}
