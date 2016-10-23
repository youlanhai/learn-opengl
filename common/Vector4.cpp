#include "Vector4.h"
#include "Vector2.h"
#include "Vector3.h"

/*static*/ Vector4 Vector4::Zero(0, 0, 0, 0);
/*static*/ Vector4 Vector4::One(1, 1, 1, 1);
/*static*/ Vector4 Vector4::XAxis(1, 0, 0, 0);
/*static*/ Vector4 Vector4::YAxis(0, 1, 0, 0);
/*static*/ Vector4 Vector4::ZAxis(0, 0, 1, 0);

Vector4::Vector4(const Vector2 &v, float _z, float _w)
: x(v.x), y(v.y), z(_z), w(_w)
{}

Vector4::Vector4(const Vector3 &v, float _w)
: x(v.x), y(v.y), z(v.z), w(_w)
{}

void Vector4::normalize()
{
	float l = length();
	if (l != 0.0f)
	{
		*this /= l;
	}
}
