#pragma once

#include "Singleton.h"
#include "Matrix.h"
#include <vector>

class Renderer : public Singleton<Renderer>
{
public:
    Renderer();
    ~Renderer();

    void pushMatrix(const Matrix &matrix);
    void popMatrix();

	void setWorldMatrix(const Matrix &matrix);
    const Matrix& getWorldMatrix() const{ return matrixs_.back(); }

    void setViewMatrix(const Matrix &mat);
    const Matrix& getViewMatrix() const{ return matView_; }

    void setProjMatrix(const Matrix &mat);
    const Matrix& getProjMatrix() const{ return matProj_; }

    const Matrix& getWorldViewMatrix() const;
    const Matrix& getWorldViewProjMatrix() const;
    const Matrix& getViewProjMatrix() const;

private:
    std::vector<Matrix> matrixs_;
    Matrix      matView_;
    Matrix      matProj_;
    
    mutable Matrix      matViewProj_;
    mutable Matrix      matWorldViewProj_;
    mutable Matrix      matWorldView_;

	mutable uint32_t	dirtyFlag_;
};
