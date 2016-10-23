#include "Vector2.h"
#include "Vector3.h"

/*static*/ Vector2 Vector2::Zero(0, 0);
/*static*/ Vector2 Vector2::One(1, 1);
/*static*/ Vector2 Vector2::XAxis(1, 0);
/*static*/ Vector2 Vector2::YAxis(0, 1);

Vector2::Vector2(const Vector3 &v)
: x(v.x), y(v.y)
{}

void Vector2::normalize()
{
	float l = length();
	if(l != 0.0f)
	{
		*this *= 1.0f / l;
	}
}
