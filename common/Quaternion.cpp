#include "Quaternion.h"
#include "Vector4.h"

Quaternion::Quaternion(const Vector4 &v)
: x(v.x), y(v.y), z(v.z), w(v.w)
{}
