#include "Transform.h"
#include "MathDef.h"
#include "Application.h"
#include "Component.h"
#include <algorithm>
#include "Renderer.h"

Transform::Transform()
    : parent_(nullptr)
    , dirtyFlag_(DIRTY_ALL)
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

Matrix Transform::getLocalToWorldMatrix() const
{
    Matrix matrix = getModelMatrix();

    Transform *p = parent_;
    while (p != nullptr)
    {
        matrix.postMultiply(p->getModelMatrix());
        p = p->getParent();
    }

    return matrix;
}

Matrix Transform::getWorldToLocalMatrix() const
{
    Matrix matrix = getLocalToWorldMatrix();
    matrix.invert();
    return matrix;
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
    child->parent_ = this;
    children_.push_back(TransformPair(true, child));
}

std::vector<TransformPtr> Transform::getChildren() const
{
    std::vector<TransformPtr> ret;
    ret.reserve(children_.size());

    for (auto &pair : children_)
    {
        if (pair.first)
        {
            ret.push_back(pair.second);
        }
    }
    return ret;
}

TransformPtr Transform::getChildByName(const std::string & name)
{
    for (auto &pair : children_)
    {
        if (pair.first && pair.second->getName() == name)
        {
            return pair.second;
        }
    }
    return nullptr;
}

void Transform::removeChild(TransformPtr child)
{
    for (auto &pair : children_)
    {
        if (pair.first && pair.second == child)
        {
            pair.first = false;
            pair.second->parent_ = nullptr;
            childrenDirty_ = true;
            break;
        }
    }
}

void Transform::removeChildByName(const std::string & name)
{
    for (auto &pair : children_)
    {
        if (pair.first && pair.second->getName() == name)
        {
            pair.first = false;
            pair.second->parent_ = nullptr;
            childrenDirty_ = true;
            break;
        }
    }
}

void Transform::removeChildByIndex(int index)
{
    TransformPair &pair = children_[index];
    pair.first = false;
    pair.second->parent_ = nullptr;
    childrenDirty_ = true;
}


void Transform::addComponent(ComponentPtr com)
{
    com->setTransfrom(this);
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
            pair.first = false;
            pair.second->setTransfrom(nullptr);
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

    for (auto & pair : children_)
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
    if (childrenDirty_)
    {
        removeUnusedChildren();
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

    for (auto & pair : children_)
    {
        if (pair.first)
        {
            pair.second->draw(renderer);
        }
    }

    renderer->popMatrix();
}

void Transform::removeUnusedComponents()
{
    componentDirty_ = false;
    auto it = std::remove_if(components_.begin(), components_.end(), [](ComponentPair &pair) { return !pair.first; });
    components_.erase(it, components_.end());
}

void Transform::removeUnusedChildren()
{
    childrenDirty_ = false;
    auto it = std::remove_if(children_.begin(), children_.end(), [](TransformPair &pair) { return !pair.first; });
    children_.erase(it, children_.end());
}
