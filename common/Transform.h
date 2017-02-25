#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "Reference.h"
#include "SmartPointer.h"
#include "Matrix.h"
#include <vector>
#include <typeinfo>

class Component;
typedef SmartPointer<Component> ComponentPtr;

class Renderer;

class Transform;
typedef SmartPointer<Transform> TransformPtr;

class Transform : public ReferenceCount
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

    const std::string& getName() const { return name_; }
    void setName(const std::string & name) { name_ = name; }
    
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

public: // 子结点相关功能

    void addChild(TransformPtr child);
    int getNumChildren() const { return children_.size(); }
    /** 返回所有有效的直属子结点。*/
    std::vector<TransformPtr> getChildren() const;
    TransformPtr getChildByName(const std::string &name);
    TransformPtr getChildByIndex(int index);

    /** remove操作不会立即生效，会等到帧末尾才执行。*/
    void removeChild(TransformPtr child);
    void removeChildByName(const std::string &name);
    void removeChildByIndex(int index);

public: // 组件相关功能

    void addComponent(ComponentPtr com);
    int getNumComponents() const { return components_.size(); }
    /** 返回所有有效的组件。*/
    std::vector<ComponentPtr> getComponents() const;
    ComponentPtr getComponentByType(const std::type_info &info);
    ComponentPtr getComponentByName(const std::string &name);
    /** remove操作不会立即生效，会等到帧末尾才执行。*/
    void removeComponent(ComponentPtr com);

    void tick(float elapse);
    void draw(Renderer *renderer);

protected:
    void removeUnusedComponents();
    void removeUnusedChildren();

    std::string     name_;

    mutable uint32_t dirtyFlag_;
    mutable Matrix  matModel_;

    Vector3         position_;
    Vector3         rotation_;
    Vector3         scale_;
    Matrix          matRotation_;

    typedef std::pair<bool, TransformPtr> TransformPair;
    std::vector<TransformPair>  children_;

    typedef std::pair<bool, ComponentPtr> ComponentPair;
	std::vector<ComponentPair>	components_;

    bool            childrenDirty_;
    bool            componentDirty_;
};

#endif //TRANSFORM_H
