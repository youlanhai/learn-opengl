#include "Vector3.h"
#include "Vector2.h"
#include "Vector4.h"

/*static*/ Vector3 Vector3::Zero(0, 0, 0);
/*static*/ Vector3 Vector3::One(1, 1, 1);
/*static*/ Vector3 Vector3::XAxis(1, 0, 0);
/*static*/ Vector3 Vector3::YAxis(0, 1, 0);
/*static*/ Vector3 Vector3::ZAxis(0, 0, 1);

Vector3::Vector3(const Vector2 &v, float _z)
: x(v.x), y(v.y), z(_z)
{}

Vector3::Vector3(const Vector4 &v)
: x(v.x), y(v.y), z(v.z)
{}

void Vector3::normalize()
{
	double l = std::sqrt(double(x) * x + y * y + z * z);
	if(l != 0.0f)
	{
		float d = float(1.0 / l);

		x *= d;
		y *= d;
		z *= d;
	}
}

Vector3 Vector3::crossProduct(const Vector3 &right) const
{
	Vector3 ret;
	ret.crossProduct(*this, right);
	return ret;
}

void Vector3::crossProduct(const Vector3 &left, const Vector3 &right)
{
	x = left.y * right.z - left.z * right.y;
	y = left.z * right.x - left.x * right.z;
	z = left.x * right.y - left.y * right.x;
}
