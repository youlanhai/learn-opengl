#include "Camera.h"
#include "MathDef.h"
#include "Application.h"
#include "Ray.h"

Camera::Camera()
	: moveSpeed_(1.0f)
    , zNear_(1.0f)
    , zFar_(100.0f)
    , fov_(PI_HALF)
    , aspect_(1.0f)
{
	matRotation_.setIdentity();
	matProj_.setIdentity();
}

Camera::~Camera()
{
}

void Camera::setPerspective(float fov, float aspect, float znear, float zfar)
{
    zNear_ = znear;
    zFar_ = zfar;
    fov_ = fov;
    aspect_ = aspect;
	matProj_.perspectiveProjectionGL(fov, aspect, znear, zfar);
}

void Camera::setOrtho(float w, float h, float zn, float zf)
{
    zNear_ = zn;
    zFar_ = zf;
    orthoSize_.set(w, h);
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
    matViewProj_ = getViewMatrix() * matProj_;
	return matViewProj_;
}

bool Camera::handleMouseButton(int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_RIGHT)
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


Ray Camera::screenPosToWorldRay(float x, float y) const
{
    Vector2 winSize = gApp->getWindowSize();
    x = x / winSize.x * 2.0f - 1.0f;
    y = 1.0f - y / winSize.y * 2.0f;
    return projectionPosToWorldRay(x, y);
}

Ray Camera::projectionPosToWorldRay(float x, float y) const
{
    float halfHeight = zNear_ * tan(fov_ / 2.0f);
    float halfWidth = halfHeight * aspect_;

    Vector3 dirInView(x * halfWidth, y * halfHeight, zNear_);

    Matrix mat = getLocalToWorldMatrix();

    Ray ray;
    ray.direction_ = mat.transformNormal(dirInView);
    ray.direction_.normalize();
    ray.origin_ = mat.transformPoint(Vector3::Zero);
    return ray;
}
