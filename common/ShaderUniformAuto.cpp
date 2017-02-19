#include "ShaderUniformAuto.h"
#include "Renderer.h"
#include "Camera.h"


ShaderAutoUniformProxy::ShaderAutoUniformProxy(ShaderUniformApplyFun fun)
: fun_(fun)
{
}

ShaderAutoUniformProxy::~ShaderAutoUniformProxy()
{

}

void ShaderAutoUniformProxy::apply(ShaderUniform *pUniform)
{
    (*fun_)(pUniform);
}

//////////////////////////////////////////////////////////////////

void shaderApplyWorld(ShaderUniform *pUniform)
{
    pUniform->bindValue(Renderer::instance()->getWorldMatrix());
}

void shaderApplyView(ShaderUniform *pUniform)
{
    pUniform->bindValue(Renderer::instance()->getViewMatrix());
}

void shaderApplyProj(ShaderUniform *pUniform)
{
    pUniform->bindValue(Renderer::instance()->getProjMatrix());
}

void shaderApplyViewProj(ShaderUniform *pUniform)
{
    pUniform->bindValue(Renderer::instance()->getViewProjMatrix());
}

void shaderApplyWorldView(ShaderUniform *pUniform)
{
    pUniform->bindValue(Renderer::instance()->getWorldViewMatrix());
}

void shaderApplyWorldViewProj(ShaderUniform *pUniform)
{
    pUniform->bindValue(Renderer::instance()->getWorldViewProjMatrix());
}

void shaderApplyAmbient(ShaderUniform *pUniform)
{
	pUniform->bindValue(Renderer::instance()->getAmbientColor());
}

void shaderApplyOmitLight(ShaderUniform *pUniform)
{
    assert(0);
}

void shaderApplyDirLight(ShaderUniform *pUniform)
{
    assert(0);
}

void shaderApplySpotLight(ShaderUniform *pUniform)
{
    assert(0);
}

void shaderApplyCameraPos(ShaderUniform *pUniform)
{
	auto camera = Renderer::instance()->getCamera();
	if (camera)
	{
		pUniform->bindValue(camera->getPosition());
	}
	else
	{
		pUniform->bindValue(Vector3::Zero);
	}
}

//////////////////////////////////////////////////////////////////
void registerDefaultAutoShaderUniform()
{
#define REG_SHADER_CONST_FACTORY(TYPE, FACTORY) \
    ShaderAutoUniform::set(TYPE, new ShaderAutoUniformProxy(FACTORY))

    REG_SHADER_CONST_FACTORY(AutoUniform::World, shaderApplyWorld);
    REG_SHADER_CONST_FACTORY(AutoUniform::View, shaderApplyView);
    REG_SHADER_CONST_FACTORY(AutoUniform::Proj, shaderApplyProj);
    REG_SHADER_CONST_FACTORY(AutoUniform::ViewProj, shaderApplyViewProj);
    REG_SHADER_CONST_FACTORY(AutoUniform::WorldView, shaderApplyWorldView);
    REG_SHADER_CONST_FACTORY(AutoUniform::WorldViewProj, shaderApplyWorldViewProj);
    REG_SHADER_CONST_FACTORY(AutoUniform::AmbientColor, shaderApplyAmbient);
    REG_SHADER_CONST_FACTORY(AutoUniform::OmitLight, shaderApplyOmitLight);
    REG_SHADER_CONST_FACTORY(AutoUniform::DirLight, shaderApplyDirLight);
    REG_SHADER_CONST_FACTORY(AutoUniform::SpotLight, shaderApplySpotLight);
	REG_SHADER_CONST_FACTORY(AutoUniform::CameraPos, shaderApplyCameraPos);

#undef REG_SHADER_CONST_FACTORY
}


