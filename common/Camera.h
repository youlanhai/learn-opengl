#pragma once

#include "Transform.h"
#include "Vector2.h"
#include <cstdint>

class Ray;

class Camera : public Transform
{
public:
	Camera();
	~Camera();

	void setMoveSpeed(float speed) { moveSpeed_ = speed; }
	float getMoveSpeed() const { return moveSpeed_; }

	void setPerspective(float fov, float aspect, float znear, float zfar);
	void setOrtho(float w, float h, float zn, float zf);

	const Matrix& getProjMatrix() const { return matProj_; }
	const Matrix& getViewMatrix() const;
	const Matrix& getViewProjMatrix() const;

	bool handleMouseButton(int button, int action, int mods);
	bool handleMouseMove(float x, float y);
	bool handleMouseScroll(float xoffset, float yoffset);
	bool handleCameraMove();
    
    float getZNear() const { return zNear_; }
    float getZFar() const { return zFar_; }
    float getFov() const { return fov_; }
    float getAspect() const { return aspect_; }
    const Vector2& getOrthoSize() const { return orthoSize_; }
    
    Ray screenPosToWorldRay(float x, float y) const;
    Ray projectionPosToWorldRay(float x, float y) const;

private:
	mutable Matrix	matView_;
	mutable Matrix  matViewProj_;
	Matrix	matProj_;

	float	moveSpeed_;
	Vector2 lastCursorPos_;
    
    float   zNear_;
    float   zFar_;
    Vector2 orthoSize_;
    
    float   fov_;
    float   aspect_;
};
