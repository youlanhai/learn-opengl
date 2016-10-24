#ifndef QUTERNION_H
#define QUTERNION_H

#include <cstddef>

class Vector4;

class Quaternion
{
public:
	float x, y, z, w;

	Quaternion() : x(0), y(0), z(0), w(0) {}
	Quaternion(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
	explicit Quaternion(const Vector4 &v);

	float& operator [] (size_t i){ return reinterpret_cast<float*>(this)[i]; }
	float  operator [] (size_t i) const { return reinterpret_cast<const float*>(this)[i]; }
};

#endif //QUTERNION_H
