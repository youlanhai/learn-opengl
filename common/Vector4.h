#ifndef VECTOR4_H
#define VECTOR4_H

#include <cmath>

class Vector2;
class Vector3;

class Vector4
{
public:
	float x, y, z, w;

	Vector4() : x(0), y(0), z(0), w(0) {}
	explicit Vector4(float v) : x(v), y(v), z(v), w(v) {}
	Vector4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
	Vector4(const Vector2 &v, float _z, float _w);
	Vector4(const Vector3 &v, float _w);

	float& operator [] (size_t i){ return reinterpret_cast<float*>(this)[i]; }
	float  operator [] (size_t i) const { return reinterpret_cast<const float*>(this)[i]; }

	float lengthSq() const { return x * x + y * y + z *z + w * w; }
	float length() const { return sqrtf(lengthSq()); }

	void zero() { *this = Zero; }
	void set(float _x, float _y, float _z, float _w){ x = _x; y = _y; z = _z; w = _w; }

	void normalize();

	float dotProduct(const Vector4 &right) const { return x * right.x + y * right.y + z * right.z + w * right.w; }

	Vector4 operator + (const Vector4 &v) const { return Vector4(x + v.x, y + v.y, z + v.z, w + v.w); }
	Vector4 operator - (const Vector4 &v) const { return Vector4(x - v.x, y - v.y, z - v.z, w - v.w); }
	Vector4 operator * (const Vector4 &v) const { return Vector4(x * v.x, y * v.y, z * v.z, w * v.w); }
	Vector4 operator / (const Vector4 &v) const { return Vector4(x / v.x, y / v.y, z / v.z, w / v.w); }

	Vector4 operator * (float v) const { return Vector4(x * v, y * v, z * v, w * v); }
	Vector4 operator / (float v) const { *this * (1.0f / v); }

	const Vector4& operator += (const Vector4 &v) { x += v.x; y += v.y; z += v.z; w += v.w; return *this; }
	const Vector4& operator -= (const Vector4 &v) { x -= v.x; y -= v.y; z -= v.z; w -= v.w; return *this; }
	const Vector4& operator *= (const Vector4 &v) { x *= v.x; y *= v.y; z *= v.z; w *= v.w; return *this; }
	const Vector4& operator /= (const Vector4 &v) { x /= v.x; y /= v.y; z /= v.z; w /= v.w; return *this; }

	const Vector4& operator *= (float v) { x *= v, y *= v; z *= v; w *= v; return *this; }
	const Vector4& operator /= (float v) { return *this *= (1.0f / v); }
	
public:
	static Vector4 Zero;
	static Vector4 One;
	static Vector4 XAxis;
	static Vector4 YAxis;
	static Vector4 ZAxis;
};

#endif // VECTOR4_H
