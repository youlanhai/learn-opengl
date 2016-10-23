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
	float l = length();
	if(l != 0.0f)
	{
		*this *= 1.0f / l;
	}
}

void Vector3::crossProduct(const Vector3 &left, const Vector3 &right)
{
	float _x = left.y * right.z - left.z * right.y;
	float _y = left.z * right.z - left.x * right.z;
	float _z = left.x * right.y - left.y * right.z;
	x = _x;
	y = _y;
	z = _z;
}
