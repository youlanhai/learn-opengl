#pragma once

#include "Reference.h"
#include "SmartPointer.h"
#include <string>

class Transform;
class Renderer;

class Component : public ReferenceCount
{
public:
    Component();
    ~Component();

    virtual void tick(float elapse) {}
    virtual void draw(Renderer *renderer) {}

    const std::string& getName() const { return name_; }
    void setName(const std::string & name) { name_ = name; }

    void setTransfrom(Transform *transform) { transform_ = transform; }
    Transform* getTransform() { return transform_; }

protected:
    std::string     name_;
    Transform*      transform_;
};

typedef SmartPointer<Component> ComponentPtr;
