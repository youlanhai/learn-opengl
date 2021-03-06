#include "Application.h"
#include "ShaderProgram.h"
#include "ShaderProgramMgr.h"
#include "ShaderUniform.h"
#include "TextureMgr.h"
#include "FileSystem.h"
#include "DemoTool.h"
#include "PathTool.h"
#include "LogTool.h"
#include "Matrix.h"
#include "Mesh.h"

const float PI = 3.141592f;

class MyApplication : public Application
{
public:

	MyApplication()
	{
		glfwWindowHint(GLFW_SAMPLES, 4);
	}

	bool onCreate() override
	{
		std::string resPath = findResPath();
		FileSystem::instance()->addSearchPath(resPath);
		FileSystem::instance()->addSearchPath(joinPath(resPath, "common"));
		FileSystem::instance()->dumpSearchPath();

		TexturePtr texture = TextureMgr::instance()->get("rock.png");
		TexturePtr texNormalMap = TextureMgr::instance()->get("rock-normal.png");
		if (!texture || !texNormalMap)
		{
			return false;
		}
		texture->setQuality(TextureQuality::TwoLinear);
		texNormalMap->setQuality(TextureQuality::ThreeLinear);

		const char *ShaderFile = "shader/normalmap.shader";
		shader_ = ShaderProgramMgr::instance()->get(ShaderFile);
		if (!shader_)
		{
			return false;
		}

		shader_->bind();
		ShaderUniform *un;

		un = shader_->findUniform("u_texture0");
		if (un) un->bindValue(texture.get());

		un = shader_->findUniform("u_texture1");
		if (un) un->bindValue(texNormalMap.get());

		Vector3 lightDir(1, 1, 0);
		lightDir.normalize();

		un = shader_->findUniform("lightDir");
		if (un) un->bindValue(lightDir);

		un = shader_->findUniform("lightColor");
		if (un) un->bindValue(Vector3(1.5f));

		un = shader_->findUniform("u_ambientColor");
		if (un) un->bindValue(Vector3(0.2f));

		//mesh_ = createSimpleGround(Vector2(2.0f, 2.0f), 0.4f, 0.1f, 4.0f);
		//mesh_ = createPlane(Vector2(1.0f, 1.0f), 0.2f);
		mesh_ = createCube(Vector3(1.0f, 1.0f, 1.0f));
		MaterialPtr material = new Material();
		material->setShader(shader_);
		material->setAutoBindUniform(false);
		mesh_->addMaterial(material);

		setupViewProjMatrix();

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		return true;
	}

	void setupWorldMatrix()
	{
		shader_->bind();

		Matrix matWorld;
		matWorld.setRotateY(glfwGetTime() * 0.5f);

		ShaderUniform *un;

		un = shader_->findUniform("u_matWorld");
		if (un) un->bindValue(matWorld);

		un = shader_->findUniform("u_matWorldViewProj");
		if (un) un->bindValue(matWorld * matViewProj_);

		shader_->unbind();
	}

	void onDraw(Renderer *renderer) override
	{
		Application::onDraw(renderer);

		setupWorldMatrix();

		mesh_->draw(renderer);
	}

	void onSizeChange(int width, int height) override
	{
		Application::onSizeChange(width, height);
		setupViewProjMatrix();
	}

	void setupViewProjMatrix()
	{
		Matrix matView;
		matView.lookAt(Vector3(0, 1, -1.8f), Vector3::Zero, Vector3::YAxis);

		Matrix matProj;
		matProj.setIdentity();
		matProj.perspectiveProjectionGL(PI / 2.0f, getAspect(), 1.0f, 1000.0f);

		matViewProj_ = matView * matProj;
	}

	ShaderProgramPtr shader_;
	MeshPtr     mesh_;
	Matrix		matViewProj_;
};

int main()
{
    MyApplication app;
    if(app.createWindow(800, 600, "008-normal-map"))
    {
        app.mainLoop();
    }
    return 0;
}
