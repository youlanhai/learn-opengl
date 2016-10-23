#ifndef VECTOR2_H
#define VECTOR2_H

#include <cmath>

class Vector3;

class Vector2
{
public:
	float x, y;

	Vector2() : x(0), y(0) {}
	explicit Vector2(float v) : x(v), y(v) {}
	Vector2(float _x, float _y) : x(_x), y(_y) {}
	explicit Vector2(const Vector3 &v);

	float& operator [] (size_t i){ return reinterpret_cast<float*>(this)[i]; }
	float  operator [] (size_t i) const { return reinterpret_cast<const float*>(this)[i]; }

	float lengthSq() const { return x * x + y * y; }
	float length() const { return sqrtf(lengthSq()); }

	void zero() { *this = Zero; }
	void set(float _x, float _y){ x = _x; y = _y; }

	void normalize();

	Vector2 operator + (const Vector2 &v) const { return Vector2(x + v.x, y + v.y); }
	Vector2 operator - (const Vector2 &v) const { return Vector2(x - v.x, y - v.y); }
	Vector2 operator * (const Vector2 &v) const { return Vector2(x * v.x, y * v.y); }
	Vector2 operator / (const Vector2 &v) const { return Vector2(x / v.x, y / v.y); }

	Vector2 operator * (float v) const { return Vector2(x * v, y * v); }
	Vector2 operator / (float v) const { return *this *  (1.0f / v); }

	const Vector2& operator += (const Vector2 &v) { x += v.x; y += v.y; return *this; }
	const Vector2& operator -= (const Vector2 &v) { x -= v.x; y -= v.y; return *this; }
	const Vector2& operator *= (const Vector2 &v) { x *= v.x; y *= v.y; return *this; }
	const Vector2& operator /= (const Vector2 &v) { x /= v.x; y /= v.y; return *this; }

	const Vector2& operator *= (float v) { x *= v, y *= v; return *this; }
	const Vector2& operator /= (float v) { return *this *= (1.0f / v); }

public:
	static Vector2 Zero;
	static Vector2 One;
	static Vector2 XAxis;
	static Vector2 YAxis;
};

#endif // VECTOR2_H
