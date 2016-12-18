#pragma once

#include "Matrix.h"
#include "Vector2.h"
#include <cstdint>

class Camera
{
public:
	Camera();
	~Camera();

	void setMoveSpeed(float speed) { moveSpeed_ = speed; }
	float getMoveSpeed() const { return moveSpeed_; }
	
	void setPosition(const Vector3 &position);
	const Vector3& getPosition() const { return position_; }

	void translate(const Vector3 &delta);

	void setRotation(float pitch, float yaw, float roll);
	void setRotation(const Vector3 &rotation);
	const Vector3& getRotation() const { return rotation_;  }
	const Matrix& getRotationMatrix() const { return matRotation_; }

	const Vector3& getRightVector() const { return matRotation_[0]; }
	const Vector3& getUpVector() const { return matRotation_[1]; }
	const Vector3& getForwardVector() const { return matRotation_[2]; }

	void lookAt(const Vector3& position, const Vector3 &target, const Vector3 &up);

	void setPerspective(float fov, float aspect, float znear, float zfar);

	const Matrix& getProjMatrix() const { return matProj_; }
	const Matrix& getViewMatrix() const;
	const Matrix& getViewProjMatrix() const;

	bool handleMouseButton(int button, int action, int mods);
	bool handleMouseMove(float x, float y);
	bool handleMouseScroll(float xoffset, float yoffset);
	bool handleCameraMove();

private:
	mutable Matrix	matView_;
	mutable Matrix  matViewProj_;
	Matrix	matProj_;

	mutable uint32_t dirtyFlag_;

	Vector3 position_;
	Vector3 rotation_;
	Matrix  matRotation_;

	float	moveSpeed_;
	Vector2 lastCursorPos_;
};
