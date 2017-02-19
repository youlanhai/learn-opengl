#ifndef MATRIX_H
#define MATRIX_H

#include "MathDef.h"
#include "Vector3.h"
#include "Vector4.h"

class Quaternion;

class Matrix
{
public:
    // 矩阵使用行矩阵存贮。用于以后兼容d3d。
    union
    {
        float m[4][4];
        float _m[16];
        struct {
            float _11, _12, _13, _14;
            float _21, _22, _23, _24;
            float _31, _32, _33, _34;
            float _41, _42, _43, _44;
        };
    };

    // 处于效率考虑。Matrix不进行默认构造，需要手动调用setIdentity设置成标准矩阵。
    Matrix(){}
	explicit Matrix(float *data);
    Matrix(float v11, float v12, float v13, float v14,
            float v21, float v22, float v23, float v24,
            float v31, float v32, float v33, float v34,
            float v41, float v42, float v43, float v44);
    Matrix(const Vector4 &v1, const Vector4 &v2, const Vector4 &v3, const Vector4 &v4);
    Matrix(const Vector3 &x, const Vector3 &y, const Vector3 &z);

    Vector3& operator[] (size_t i){ return *reinterpret_cast<Vector3*>(m[i]); }
    const Vector3& operator[] (size_t i) const { return *reinterpret_cast<const Vector3*>(m[i]); }

    void setIdentity(){ *this = Identity; }
    void setZero(){ *this = Zero; }

    void setScale( const float x, const float y, const float z );
    void setScale( const Vector3 & scale );
	void setScale( float scale );

    void setTranslate( const float x, const float y, const float z );
    void setTranslate( const Vector3 & pos );

    void setRotateX( const float angle );
    void setRotateY( const float angle );
    void setRotateZ( const float angle );
    void setRotate( const Quaternion & q );

	void setRotate(float x, float y, float z);
	Vector3 getRotate() const;

    void multiply( const Matrix& m1, const Matrix& m2 );
    void preMultiply( const Matrix& m );
    void postMultiply( const Matrix& m );

    void invertOrthonormal( const Matrix& m );
    void invertOrthonormal();
    bool invert( const Matrix& m );
    bool invert();
    float getDeterminant() const;

    void transpose( const Matrix & m );
    void transpose();

	void decompose(Quaternion & rotation, Vector3 & scale, Vector3 & translate) const;

    Matrix operator * (const Matrix &v) const { Matrix m; m.multiply(*this, v); return m; }
    Matrix operator + (const Matrix &v) const;
    Matrix operator - (const Matrix &v) const;

    Matrix operator * (float v) const;
    Matrix operator / (float v) const;

    const Matrix& operator *= (const Matrix &v){ postMultiply(v); return *this; }
    const Matrix& operator += (const Matrix &v);
    const Matrix& operator -= (const Matrix &v);

    const Matrix& operator *= (float v);
    const Matrix& operator /= (float v);

	void lookAt(const Vector3& position, const Vector3& target, const Vector3& up);

    void orthogonalProjection( float w, float h, float zn, float zf );
    void perspectiveProjection( float fov, float aspectRatio,
                                float nearPlane, float farPlane );

    // GL的视椎体z取值为[-1, 1]，与dx的[0, 1]不同，所以投影矩阵也不同
    void perspectiveProjectionGL( float fov, float aspectRatio, float nearPlane, float farPlane );
    void orthogonalProjectionGL(float w, float h, float zn, float zf);
    void orthogonalProjectionOffCenterGL(float left, float right, float bottom, float top, float zn, float zfar);

public:
    static Matrix Identity;
    static Matrix Zero;
};


inline void Matrix::setScale( const Vector3 & scale )
{
    setScale(scale.x, scale.y, scale.z);
}

inline void Matrix::setScale(float scale)
{
	setScale(scale, scale, scale);
}

inline void Matrix::setTranslate( const Vector3 & pos )
{
    setTranslate(pos.x, pos.y, pos.z);
}

inline void Matrix::preMultiply( const Matrix& m )
{
    Matrix m2 = *this;
    multiply(m, m2);
}

inline void Matrix::postMultiply( const Matrix& m )
{
    Matrix m0 = *this;
    multiply(m0, m);
}

inline void Matrix::invertOrthonormal()
{
    Matrix m(*this);
    invertOrthonormal(m);
}

inline bool Matrix::invert()
{
    Matrix m(*this);
    return invert(m);
}

inline void Matrix::transpose()
{
    Matrix m = *this;
    transpose(m);
}


#endif //MATRIX_H
