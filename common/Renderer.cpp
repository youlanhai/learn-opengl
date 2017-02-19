#include "Renderer.h"
#include "ShaderUniformAuto.h"

IMPLEMENT_SINGLETON(Renderer);

enum DirtyFlag
{
	DF_WORLD_VIEW_PROJ = 1 << 0,
	DF_WORLD_VIEW = 1 << 1,
	DF_VIEW_PROJ = 1 << 2,

	DF_ALL = DF_WORLD_VIEW_PROJ | DF_WORLD_VIEW | DF_VIEW_PROJ,
};


Renderer::Renderer()
	: dirtyFlag_(DF_ALL)
	, matView_(Matrix::Identity)
	, matProj_(Matrix::Identity)
{
	registerDefaultAutoShaderUniform();
	pushMatrix(Matrix::Identity);
}

Renderer::~Renderer()
{

}

void Renderer::pushMatrix(const Matrix &matrix)
{
	matrixs_.push_back(matrix);
	dirtyFlag_ |= DF_WORLD_VIEW | DF_WORLD_VIEW_PROJ;
}

void Renderer::popMatrix()
{
	matrixs_.pop_back();
	dirtyFlag_ |= DF_WORLD_VIEW | DF_WORLD_VIEW_PROJ;
}

void Renderer::setWorldMatrix(const Matrix & matrix)
{
	matrixs_.back() = matrix;
	dirtyFlag_ |= DF_WORLD_VIEW | DF_WORLD_VIEW_PROJ;
}

void Renderer::setViewMatrix(const Matrix &mat)
{
	matView_ = mat;
	dirtyFlag_ |= DF_WORLD_VIEW | DF_WORLD_VIEW_PROJ | DF_VIEW_PROJ;
}

void Renderer::setProjMatrix(const Matrix &mat)
{
	matProj_ = mat;
	dirtyFlag_ |= DF_WORLD_VIEW_PROJ | DF_VIEW_PROJ;
}

const Matrix& Renderer::getWorldViewMatrix() const
{
	if (dirtyFlag_ & DF_WORLD_VIEW)
	{
		dirtyFlag_ &= ~uint32_t(DF_WORLD_VIEW);
		matWorldView_.multiply(matrixs_.back(), matView_);
	}
	return matWorldView_;
}

const Matrix& Renderer::getWorldViewProjMatrix() const
{
	if (dirtyFlag_ & DF_WORLD_VIEW_PROJ)
	{
		dirtyFlag_ &= ~uint32_t(DF_WORLD_VIEW_PROJ);
		matWorldViewProj_.multiply(getWorldViewMatrix(), matProj_);
	}
	return matWorldViewProj_;
}

const Matrix& Renderer::getViewProjMatrix() const
{
	if (dirtyFlag_ & DF_VIEW_PROJ)
	{
		dirtyFlag_ &= ~uint32_t(DF_VIEW_PROJ);
		matViewProj_.multiply(matView_, matProj_);
	}
	return matViewProj_;
}

