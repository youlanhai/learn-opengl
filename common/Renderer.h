#pragma once

#include "Singleton.h"
#include "Matrix.h"
#include "Color.h"
#include "SmartPointer.h"
#include <vector>

class Camera;
class Material;
typedef SmartPointer<Material> MaterialPtr;

class Renderer : public Singleton<Renderer>
{
public:
    Renderer();
    ~Renderer();

    void pushMatrix(const Matrix &matrix);
    void pushMatrix();
    void popMatrix();

	void setWorldMatrix(const Matrix &matrix);
    const Matrix& getWorldMatrix() const{ return matrixs_.back(); }
    Matrix& getWorldMatrix() { return matrixs_.back(); }

    void setViewMatrix(const Matrix &mat);
    const Matrix& getViewMatrix() const{ return matView_; }

    void setProjMatrix(const Matrix &mat);
    const Matrix& getProjMatrix() const{ return matProj_; }

    const Matrix& getWorldViewMatrix() const;
    const Matrix& getWorldViewProjMatrix() const;
    const Matrix& getViewProjMatrix() const;

    void setCamera(Camera *camera);
	Camera* getCamera() { return camera_; }

	void applyCameraMatrix();

	void setAmbientColor(const Color &color) { ambientColor_ = color; }
	const Color& getAmbientColor() const { return ambientColor_; }

	void setZWriteEnable(bool enable);
	void setColorWriteEnable(bool enable);

    bool beginDraw();
    void endDraw();

    void setOverwriteMaterial(MaterialPtr mtl);
    MaterialPtr getOverwriteMaterial();

private:
    std::vector<Matrix> matrixs_;
    Matrix      matView_;
    Matrix      matProj_;
	Camera*		camera_;
	Color		ambientColor_;

    MaterialPtr overwiteMaterial_;
    
    mutable Matrix      matViewProj_;
    mutable Matrix      matWorldViewProj_;
    mutable Matrix      matWorldView_;

	mutable uint32_t	dirtyFlag_;
};
