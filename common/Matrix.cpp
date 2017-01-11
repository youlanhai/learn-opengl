#include <cstring>
#include <cmath>
#include <cassert>

#include "Matrix.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Quaternion.h"

/*static*/ Matrix Matrix::Identity(
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1
);

/*static*/ Matrix Matrix::Zero(
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0
);

Matrix::Matrix(float *data)
{
	memcpy(_m, data, sizeof(float) * 16);
}

Matrix::Matrix(float v11, float v12, float v13, float v14,
        float v21, float v22, float v23, float v24,
        float v31, float v32, float v33, float v34,
        float v41, float v42, float v43, float v44)
: _11(v11), _12(v12), _13(v13), _14(v14)
, _21(v21), _22(v22), _23(v23), _24(v24)
, _31(v31), _32(v32), _33(v33), _34(v34)
, _41(v41), _42(v42), _43(v43), _44(v44)
{}

Matrix::Matrix(const Vector4 &v1, const Vector4 &v2, const Vector4 &v3, const Vector4 &v4)
: _11(v1.x), _12(v1.y), _13(v1.z), _14(v1.w)
, _21(v2.x), _22(v2.y), _23(v2.z), _24(v2.w)
, _31(v3.x), _32(v3.y), _33(v3.z), _34(v3.w)
, _41(v4.x), _42(v4.y), _43(v4.z), _44(v4.w)
{}

Matrix::Matrix(const Vector3 &x, const Vector3 &y, const Vector3 &z)
: _11(x.x), _12(x.y), _13(x.z), _14(0)
, _21(y.x), _22(y.y), _23(y.z), _24(0)
, _31(z.x), _32(z.y), _33(z.z), _34(0)
, _41(0), _42(0), _43(0), _44(1)
{}

void Matrix::setScale( const float x, const float y, const float z )
{
    setIdentity();
    _11 = x;
    _22 = y;
    _33 = z;
}

void Matrix::setTranslate( const float x, const float y, const float z )
{
    setIdentity();
    _41 = x;
    _42 = y;
    _43 = z;
}

void Matrix::setRotateX( const float angle )
{
    setIdentity();
    float sa = (float) sin(angle);
    float ca = (float) cos(angle);

    _22 = ca;  _23 = sa;
    _32 = -sa; _33 = ca;
}

void Matrix::setRotateY( const float angle )
{
    setIdentity();
    float sa = (float) sin(angle);
    float ca = (float) cos(angle);

    _11 = ca; _13 = -sa;
    _31 = sa; _33 = ca;
}

void Matrix::setRotateZ( const float angle )
{
    setIdentity();
    float sa = (float) sin(angle);
    float ca = (float) cos(angle);

    _11 = ca;  _12 = sa;
    _21 = -sa; _22 = ca;
}

void Matrix::setRotate( const Quaternion & q )
{
    //calculate coefficents
    float xx = q.x * q.x * 2.f;
    float xy = q.x * q.y * 2.f;
    float xz = q.x * q.z * 2.f;

    float yy = q.y * q.y * 2.f;
    float yz = q.y * q.z * 2.f;

    float zz = q.z * q.z * 2.f;

    float wx = q.w * q.x * 2.f;
    float wy = q.w * q.y * 2.f;
    float wz = q.w * q.z * 2.f;

    _11 = 1.f - (yy + zz);
    _21 = xy - wz;
    _31 = xz + wy;

    _12 = xy + wz;
    _22 = 1.f - (xx + zz);
    _32 = yz - wx;

    _13 = xz - wy;
    _23 = yz + wx;
    _33 = 1.f - (xx + yy);

    _14 = 0.f;
    _24 = 0.f;
    _34 = 0.f;

    _41 = 0.0f;
    _42 = 0.0f;
    _43 = 0.0f;
    _44 = 1.0f;
}


// pitch -> x, yaw -> y, roll -> z
void Matrix::setRotate( float pitch, float yaw, float roll )
{
	const double sxa = sin(pitch);
	const double cxa = cos(pitch);
    const double sya = sin(yaw);
    const double cya = cos(yaw);
    const double sza = sin(roll);
    const double cza = cos(roll);

    _11 = cya * cza;
    _12 = cxa * sza;
    _13 = -sya * cza + cya * sza * sxa;
    _14 = 0.f;

    _21 = -cya * sza;
    _22 = cxa * cza;
    _23 = sya * sza + cya * cza * sxa;
    _24 = 0.f;

    _31 = sya * cxa;
    _32 = -sxa;
    _33 = cxa * cya;
    _34 = 0.f;

    _41 = 0.f;
    _42 = 0.f;
    _43 = 0.f;
    _44 = 1.f;
}

void Matrix::multiply( const Matrix& m1, const Matrix& m2 )
{
    _11 = m1._11 * m2._11 + m1._12 * m2._21 + m1._13 * m2._31 + m1._14 * m2._41;
    _12 = m1._11 * m2._12 + m1._12 * m2._22 + m1._13 * m2._32 + m1._14 * m2._42;
    _13 = m1._11 * m2._13 + m1._12 * m2._23 + m1._13 * m2._33 + m1._14 * m2._43;
    _14 = m1._11 * m2._14 + m1._12 * m2._24 + m1._13 * m2._34 + m1._14 * m2._44;

    _21 = m1._21 * m2._11 + m1._22 * m2._21 + m1._23 * m2._31 + m1._24 * m2._41;
    _22 = m1._21 * m2._12 + m1._22 * m2._22 + m1._23 * m2._32 + m1._24 * m2._42;
    _23 = m1._21 * m2._13 + m1._22 * m2._23 + m1._23 * m2._33 + m1._24 * m2._43;
    _24 = m1._21 * m2._14 + m1._22 * m2._24 + m1._23 * m2._34 + m1._24 * m2._44;

    _31 = m1._31 * m2._11 + m1._32 * m2._21 + m1._33 * m2._31 + m1._34 * m2._41;
    _32 = m1._31 * m2._12 + m1._32 * m2._22 + m1._33 * m2._32 + m1._34 * m2._42;
    _33 = m1._31 * m2._13 + m1._32 * m2._23 + m1._33 * m2._33 + m1._34 * m2._43;
    _34 = m1._31 * m2._14 + m1._32 * m2._24 + m1._33 * m2._34 + m1._34 * m2._44;

    _41 = m1._41 * m2._11 + m1._42 * m2._21 + m1._43 * m2._31 + m1._44 * m2._41;
    _42 = m1._41 * m2._12 + m1._42 * m2._22 + m1._43 * m2._32 + m1._44 * m2._42;
    _43 = m1._41 * m2._13 + m1._42 * m2._23 + m1._43 * m2._33 + m1._44 * m2._43;
    _44 = m1._41 * m2._14 + m1._42 * m2._24 + m1._43 * m2._34 + m1._44 * m2._44;
}

void Matrix::invertOrthonormal( const Matrix& matrix)
{
    _11 = matrix._11;
    _12 = matrix._21;
    _13 = matrix._31;
    _14 = 0.f;

    _21 = matrix._12;
    _22 = matrix._22;
    _23 = matrix._32;
    _24 = 0.f;

    _31 = matrix._13;
    _32 = matrix._23;
    _33 = matrix._33;
    _34 = 0.f;

    _41 = -(matrix._41 * _11 + matrix._42 * _21 + matrix._43 * _31);
    _42 = -(matrix._41 * _12 + matrix._42 * _22 + matrix._43 * _32);
    _43 = -(matrix._41 * _13 + matrix._42 * _23 + matrix._43 * _33);
    _44 = 1.f;
}

bool Matrix::invert( const Matrix& matrix)
{
    float determinant = matrix.getDeterminant();
    if (determinant == 0.f)
    {
        return false;
    }

    // TODO: Need to consider the invert of a 4x4. This is for a 3x4.

    float rcp = 1 / determinant;

    _11 = matrix._22 * matrix._33 - matrix._23 * matrix._32;
    _12 = matrix._13 * matrix._32 - matrix._12 * matrix._33;
    _13 = matrix._12 * matrix._23 - matrix._13 * matrix._22;
    _21 = matrix._23 * matrix._31 - matrix._21 * matrix._33;
    _22 = matrix._11 * matrix._33 - matrix._13 * matrix._31;
    _23 = matrix._13 * matrix._21 - matrix._11 * matrix._23;
    _31 = matrix._21 * matrix._32 - matrix._22 * matrix._31;
    _32 = matrix._12 * matrix._31 - matrix._11 * matrix._32;
    _33 = matrix._11 * matrix._22 - matrix._12 * matrix._21;

    _11 *= rcp;
    _12 *= rcp;
    _13 *= rcp;

    _21 *= rcp;
    _22 *= rcp;
    _23 *= rcp;

    _31 *= rcp;
    _32 *= rcp;
    _33 *= rcp;

    _41 = -(matrix._41 * _11 + matrix._42 * _21 + matrix._43 * _31);
    _42 = -(matrix._41 * _12 + matrix._42 * _22 + matrix._43 * _32);
    _43 = -(matrix._41 * _13 + matrix._42 * _23 + matrix._43 * _33);

    if (determinant == 0)
    {
        this->setIdentity();
    }

    return true;
}

float Matrix::getDeterminant() const
{
    float det = 0;

    det += _11 * (_22 * _33 - _23 * _32);
    det -= _12 * (_21 * _33 - _23 * _31);
    det += _13 * (_21 * _32 - _22 * _31);

    return det;
}

void Matrix::transpose( const Matrix & m )
{
    assert(this != &m);
    _11 = m._11; _12 = m._21; _13 = m._31; _14 = m._41;
    _21 = m._12; _22 = m._22; _23 = m._32; _24 = m._42;
    _31 = m._13; _32 = m._23; _33 = m._33; _34 = m._43;
    _41 = m._14; _42 = m._24; _43 = m._34; _44 = m._44;
}

Matrix Matrix::operator + (const Matrix &v) const
{
    Matrix m;
    for(int i = 0; i < 16; ++i)
    {
        m._m[i] = _m[i] + v._m[i];
    }
    return m;
}

Matrix Matrix::operator - (const Matrix &v) const
{
    Matrix m;
    for(int i = 0; i < 16; ++i)
    {
        m._m[i] = _m[i] + v._m[i];
    }
    return m;
}

Matrix Matrix::operator * (float v) const
{
    Matrix m;
    for(int i = 0; i < 16; ++i)
    {
        m._m[i] = _m[i] * v;
    }
    return m;
}

Matrix Matrix::operator / (float v) const
{
    return *this * (1.0f / v);
}

const Matrix& Matrix::operator += (const Matrix &v)
{
    for(int i = 0; i < 16; ++i)
    {
        _m[i] += v._m[i];
    }
    return *this;
}

const Matrix& Matrix::operator -= (const Matrix &v)
{
    for(int i = 0; i < 16; ++i)
    {
        _m[i] -= v._m[i];
    }
    return *this;
}

const Matrix& Matrix::operator *= (float v)
{
    for(int i = 0; i < 16; ++i)
    {
        _m[i] *= v;
    }
    return *this;
}

const Matrix& Matrix::operator /= (float v)
{
    return *this *= (1.0f / v);
}

void Matrix::lookAt(const Vector3& position, const Vector3& target, const Vector3& up)
{
    Vector3 Up;
    Vector3 Right;

    Vector3 Forward = target - position;
	Forward.normalize();

    Right.crossProduct(up, Forward);
    Right.normalize();

    Up.crossProduct(Forward, Right);
	Up.normalize();

    _11 = Right.x;
    _21 = Right.y;
    _31 = Right.z;
    _41 = -position.dotProduct(Right);

    _12 = Up.x;
    _22 = Up.y;
    _32 = Up.z;
    _42 = -position.dotProduct(Up);

    _13 = Forward.x;
    _23 = Forward.y;
    _33 = Forward.z;
    _43 = -position.dotProduct(Forward);

	_14 = 0.0f;
    _24 = 0.0f;
    _34 = 0.0f;
    _44 = 1.0f;
}

void Matrix::decompose(Quaternion & rotation, Vector3 & scale, Vector3 & translate) const
{
    Matrix m(*this);

    Vector3& row0 = m[0];
    Vector3& row1 = m[1];
    Vector3& row2 = m[2];
    Vector3& row3 = m[3];

    scale.x = row0.length();
    scale.y = row1.length();
    scale.z = row2.length();

    row0 *= 1.f / scale.x;
    row1 *= 1.f / scale.y;
    row2 *= 1.f / scale.z;

    Vector3 in;
    in.crossProduct(row0, row1);
    if( in.dotProduct(row2 ) < 0 )
    {
        row2 *= -1;
        scale.z *= -1;
    }

    translate = row3;
    //rotation.fromMatrix(m);
	assert(0 && "implement this method");
}

void Matrix::orthogonalProjection( float w, float h, float zn, float zf )
{
    _11 = 2.f / w; _12 = 0.f;       _13 = 0.f;          _14 = 0.f;
    _11 = 0.f;     _12 = 2.f / h;   _13 = 0.f;          _14 = 0.f;
    _11 = 0.f;     _12 = 0.f;       _13 = 1.f / (zf - zn);  _14 = 0.f;
    _11 = 0.f;     _12 = 0.f;       _13 = zn / (zn - zf);   _14 = 1.f;
}

void Matrix::perspectiveProjection( float fov, float aspectRatio,
                                    float nearPlane, float farPlane )
{
    float cot = 1 / (float)tan(fov * 0.5f);
    float rcp = 1 / (farPlane - nearPlane);

    _11 = (cot / aspectRatio);
    _12 = 0;
    _13 = 0;
    _14 = 0;

    _21 = 0;
    _22 = cot;
    _23 = 0;
    _24 = 0;

    _31 = 0;
    _32 = 0;
    _33 = rcp * farPlane;
    _34 = 1;

    _41 = 0;
    _42 = 0;
    _43 = - rcp  * farPlane * nearPlane;
    _44 = 0;
}

void Matrix::perspectiveProjectionGL( float fov, float aspectRatio,
                                     float nearPlane, float farPlane )
{
    float yScale = 1.0f / tanf(fov * 0.5f);
    float xScale = yScale / aspectRatio;
    
    float a = (farPlane + nearPlane) / (farPlane - nearPlane);
    float b = -2.0f * farPlane * nearPlane / (farPlane - nearPlane);
    
    _11 = xScale;   _12 = 0;        _13 = 0; _14 = 0;
    _21 = 0;        _22 = yScale;   _23 = 0; _24 = 0;
    _31 = 0;        _32 = 0;        _33 = a; _34 = 1;
    _41 = 0;        _42 = 0;        _43 = b; _44 = 0;
}

// 原点落在屏幕中间，x轴向右为正，y轴向上为正
void Matrix::orthogonalProjectionGL(float w, float h, float zn, float zf)
{
    float halfW = w * 0.5f;
    float halfH = h * 0.5f;
    orthogonalProjectionOffCenterGL(-halfW, halfW, -halfH, halfH, zn, zf);
}

void Matrix::orthogonalProjectionOffCenterGL(float left, float right, float bottom, float top, float zn, float zf)
{
    float w1 = 1.f / (right - left);
    float h1 = 1.f / (top - bottom);
    float z1 = 1.f / (zf - zn);
    
    _11 = 2 * w1;
    _21 = 0;
    _31 = 0;
    _41 = -(right + left) * w1;
    
    _12 = 0;
    _22 = 2 * h1;
    _32 = 0;
    _42 = -(top + bottom) * h1;
    
    _13 = 0;
    _23 = 0;
    _33 = 2 * z1;
    _43 = -(zf + zn) * z1;
    
    _14 = 0;
    _24 = 0;
    _34 = 0;
    _44 = 1;
}
