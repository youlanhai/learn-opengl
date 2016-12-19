#include "Camera.h"
#include "MathDef.h"
#include "Application.h"

enum DirtyFlag
{
	DIRTY_VIEW = 1 << 0,
	DIRTY_PROJ = 1 << 1,

	DIRTY_ALL = DIRTY_VIEW | DIRTY_PROJ,
};

Camera::Camera()
	: dirtyFlag_(DIRTY_ALL)
	, moveSpeed_(1.0f)
{
	matRotation_.setIdentity();
	matProj_.setIdentity();
}

Camera::~Camera()
{
}

void Camera::setPosition(const Vector3 & position)
{
	position_ = position;
	dirtyFlag_ |= DIRTY_VIEW;
}

void Camera::translate(const Vector3 & delta)
{
	setPosition(position_ + delta);
}

void Camera::setRotation(float pitch, float yaw, float roll)
{
	rotation_.set(yaw, pitch, roll);
	matRotation_.setRotate(pitch, yaw, roll);
	dirtyFlag_ |= DIRTY_VIEW;
}

void Camera::setRotation(const Vector3 &rotation)
{
	rotation_ = rotation;
	matRotation_.setRotate(rotation.x, rotation.y, rotation.z);
	dirtyFlag_ |= DIRTY_VIEW;
}

void Camera::lookAt(const Vector3 & position, const Vector3 & target, const Vector3 & up)
{
	dirtyFlag_ |= DIRTY_VIEW;
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

	float yaw = atan2(forward.x, forward.z);
	float pitch = -asin(forward.y);
	float roll = 0.0f;

	const float zdirxzlen = sqrtf(forward.z * forward.z + forward.x * forward.x);
	if (zdirxzlen != 0.0f)
	{
		const float acarg = (forward.z * right.x - forward.x * right.z) / zdirxzlen;
		if (acarg <= -1.0f)
		{
			roll = PI_FULL;
		}
		else if (acarg >= 1.f)
		{
			roll = 0.0f;
		}
		else
		{
			roll = acos(acarg);
			if (forward.y < 0.f)
			{
				roll = -roll;
			}
		}
	}

	rotation_.set(pitch, yaw, roll);
}

void Camera::setPerspective(float fov, float aspect, float znear, float zfar)
{
	matProj_.perspectiveProjectionGL(fov, aspect, znear, zfar);
	dirtyFlag_ |= DIRTY_PROJ;
}

void Camera::setOrtho(float w, float h, float zn, float zf)
{
	dirtyFlag_ |= DIRTY_PROJ;
	matProj_.orthogonalProjectionGL(w, h, zn, zf);
}

const Matrix & Camera::getViewMatrix() const
{
	if (dirtyFlag_ & DIRTY_VIEW)
	{
		dirtyFlag_ &= ~DIRTY_VIEW;
		matView_.transpose(matRotation_);
		matView_._41 = -position_.dotProduct(matRotation_[0]);
		matView_._42 = -position_.dotProduct(matRotation_[1]);
		matView_._43 = -position_.dotProduct(matRotation_[2]);
	}
	return matView_;
}

const Matrix & Camera::getViewProjMatrix() const
{
	if (dirtyFlag_ & (DIRTY_PROJ | DIRTY_VIEW))
	{
		dirtyFlag_ &= ~DIRTY_PROJ;
		matViewProj_ = getViewMatrix() * matProj_;
	}
	return matViewProj_;
}

bool Camera::handleMouseButton(int button, int action, int mods)
{
	if (button = GLFW_MOUSE_BUTTON_RIGHT)
	{
		lastCursorPos_ = gApp->getCursorPos();
	}
	return false;
}

bool Camera::handleMouseMove(float x, float y)
{
	bool ret = false;
	if (gApp->isMousePress(GLFW_MOUSE_BUTTON_RIGHT))
	{
		ret = true;
		Vector2 size = gApp->getWindowSize();

		float dx = (float(x) - lastCursorPos_.x) / size.x;
		float dy = (float(y) - lastCursorPos_.y) / size.y;

		Vector3 rotation = getRotation();
		rotation.y += dx * PI_FULL;
		rotation.x += dy * PI_FULL;
		setRotation(rotation);
	}
		
	lastCursorPos_.set(float(x), float(y));
	return ret;
}

bool Camera::handleMouseScroll(float xoffset, float yoffset)
{
	translate(getForwardVector() * (yoffset * 0.1f));
	return true;
}

bool Camera::handleCameraMove()
{
	float moveDelta = gApp->getDeltaTime() * moveSpeed_;

	if (gApp->isKeyPress(GLFW_KEY_A))
	{
		translate(getRightVector() * (-moveDelta));
		return true;
	}
	else if (gApp->isKeyPress(GLFW_KEY_D))
	{
		translate(getRightVector() * moveDelta);
		return true;
	}

	if (gApp->isKeyPress(GLFW_KEY_W))
	{
		Vector3 forward = getForwardVector();
		forward.y = 0.0f;
		translate(forward * moveDelta);
		return true;
	}
	else if (gApp->isKeyPress(GLFW_KEY_S))
	{
		Vector3 forward = getForwardVector();
		forward.y = 0.0f;
		translate(forward * (-moveDelta));
		return true;
	}
	return false;
}
