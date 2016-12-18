#ifndef VECTOR3_H
#define VECTOR3_H

#include <cmath>

class Vector2;
class Vector4;

class Vector3
{
public:
	float x, y, z;

	Vector3() : x(0), y(0), z(0) {}
	explicit Vector3(float v) : x(v), y(v), z(v) {}
	Vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
	Vector3(const Vector2 &v, float _z);
	explicit Vector3(const Vector4 &v);

	float& operator [] (size_t i){ return reinterpret_cast<float*>(this)[i]; }
	float  operator [] (size_t i) const { return reinterpret_cast<const float*>(this)[i]; }

	float lengthSq() const { return x * x + y * y + z *z; }
	float length() const { return sqrtf(lengthSq()); }

	void setZero() { *this = Zero; }
	void set(float _x, float _y, float _z){ x = _x; y = _y; z = _z; }

	void normalize();

	float dotProduct(const Vector3 &right) const { return x * right.x + y * right.y + z * right.z; }

	Vector3 crossProduct(const Vector3 &right) const;
	void crossProduct(const Vector3 &left, const Vector3 &right);

	Vector3 operator + (const Vector3 &v) const { return Vector3(x + v.x, y + v.y, z + v.z); }
	Vector3 operator - (const Vector3 &v) const { return Vector3(x - v.x, y - v.y, z - v.z); }
	Vector3 operator * (const Vector3 &v) const { return Vector3(x * v.x, y * v.y, z * v.z); }
	Vector3 operator / (const Vector3 &v) const { return Vector3(x / v.x, y / v.y, z / v.z); }

	Vector3 operator * (float v) const { return Vector3(x * v, y * v, z * v); }
	Vector3 operator / (float v) const { return *this * (1.0f / v); }

	const Vector3& operator += (const Vector3 &v) { x += v.x; y += v.y; z += v.z; return *this; }
	const Vector3& operator -= (const Vector3 &v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
	const Vector3& operator *= (const Vector3 &v) { x *= v.x; y *= v.y; z *= v.z; return *this; }
	const Vector3& operator /= (const Vector3 &v) { x /= v.x; y /= v.y; z /= v.z; return *this; }

	const Vector3& operator *= (float v) { x *= v, y *= v; z *= v; return *this; }
	const Vector3& operator /= (float v) { return *this *= (1.0f / v); }
	
public:
	static Vector3 Zero;
	static Vector3 One;
	static Vector3 XAxis;
	static Vector3 YAxis;
	static Vector3 ZAxis;
};

#endif // VECTOR3_H
