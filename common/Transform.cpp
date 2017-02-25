#include "Transform.h"
#include "MathDef.h"
#include "Application.h"
#include "Component.h"
#include <algorithm>
#include "Renderer.h"

Transform::Transform()
    : dirtyFlag_(DIRTY_ALL)
    , scale_(Vector3::One)
    , matRotation_(Matrix::Identity)
    , componentDirty_(false)
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

const Matrix& Transform::getModelMatrix() const
{
    if (dirtyFlag_ & DIRTY_MODEL)
    {
        dirtyFlag_ &= ~DIRTY_MODEL;

        Matrix matScale;
        matScale.setScale(scale_);

        matModel_.multiply(matRotation_, matScale);
        matModel_[3] = position_;
    }
    return matModel_;
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

void Transform::addChild(TransformPtr child)
{
}

std::vector<TransformPtr> Transform::getChildren() const
{
    return std::vector<TransformPtr>();
}

TransformPtr Transform::getChildByName(const std::string & name)
{
    return TransformPtr();
}

TransformPtr Transform::getChildByIndex(int index)
{
    return TransformPtr();
}

void Transform::removeChild(TransformPtr child)
{
}

void Transform::removeChildByName(const std::string & name)
{
}

void Transform::removeChildByIndex(int index)
{
}


void Transform::addComponent(ComponentPtr com)
{
    components_.push_back(ComponentPair(true, com));
}

std::vector<ComponentPtr> Transform::getComponents() const
{
    std::vector<ComponentPtr> ret;
    ret.reserve(components_.size());

    for (const auto &pair : components_)
    {
        if (pair.first)
        {
            ret.push_back(pair.second);
        }
    }
    return ret;
}

ComponentPtr Transform::getComponentByType(const std::type_info & info)
{
    for (ComponentPair &pair : components_)
    {
        if(pair.first && info == typeid(*pair.second))
        {
            return pair.second;
        }
    }
    return nullptr;
}

ComponentPtr Transform::getComponentByName(const std::string & name)
{
    for (ComponentPair &pair : components_)
    {
        if (pair.first && pair.second->getName() == name)
        {
            return pair.second;
        }
    }
    return nullptr;
}

void Transform::removeComponent(ComponentPtr com)
{
    for (auto &pair : components_)
    {
        if (pair.first && pair.second == com)
        {
            componentDirty_ = true;
            pair.second = false;
            break;
        }
    }
}

void Transform::tick(float elapse)
{
    for (auto &pair : components_)
    {
        if (pair.first)
        {
            pair.second->tick(elapse);
        }
    }

    if (componentDirty_)
    {
        removeUnusedComponents();
    }
}

void Transform::draw(Renderer * renderer)
{
    renderer->pushMatrix();
    renderer->getWorldMatrix().preMultiply(getModelMatrix());

    for (auto &pair : components_)
    {
        if (pair.first)
        {
            pair.second->draw(renderer);
        }
    }

    if (componentDirty_)
    {
        removeUnusedComponents();
    }

    renderer->popMatrix();
}

void Transform::removeUnusedComponents()
{
    componentDirty_ = false;
    auto it = std::remove_if(components_.begin(), components_.end(),
        [](ComponentPair &pair) {
        return !pair.first;
    });
    components_.erase(it, components_.end());
}

void Transform::removeUnusedChildren()
{
}
