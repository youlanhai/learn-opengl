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
#include "Camera.h"
#include "Model.h"
#include "Renderer.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

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

		modelShader_ = ShaderProgramMgr::instance()->get("shader/model.shader");
		if (!modelShader_)
		{
			return false;
		}

		TexturePtr texture = TextureMgr::instance()->get("rock.png");
		TexturePtr normalMap = TextureMgr::instance()->get("rock-normal.png");
		if (!texture || !normalMap)
		{
			return false;
		}
		texture->setQuality(TextureQuality::TwoLinear);
		normalMap->setQuality(TextureQuality::ThreeLinear);

		const char *ShaderFile = "shader/light_pixel.shader";
		shader_ = ShaderProgramMgr::instance()->get(ShaderFile);
		if (!shader_)
		{
			return false;
		}

		Vector3 lightDir(1, 1, 0);
		lightDir.normalize();

		modelShader_->bind();
		bindShaderUniform(modelShader_.get(), "lightDir", lightDir);
		bindShaderUniform(modelShader_.get(), "lightColor", Vector3(1.5f));
		bindShaderUniform(modelShader_.get(), "ambientColor", Vector3(0.5f));
		modelShader_->unbind();

		shader_->bind();
		bindShaderUniform(shader_.get(), "lightDir", lightDir);
		bindShaderUniform(shader_.get(), "lightColor", Vector3(1.5f));
		bindShaderUniform(shader_.get(), "ambientColor", Vector3(0.5f));
		bindShaderUniform(shader_.get(), "shininess", 64.0f);
		bindShaderUniform(shader_.get(), "specularStrength", 4.0f);
		shader_->unbind();

		mesh_ = createCube(Vector3(1.0f, 1.0f, 1.0f));
		MaterialPtr material = new Material();
		material->setShader(shader_);
		material->setTexture("u_texture0", texture.get());
		material->setTexture("u_texture1", normalMap.get());
		mesh_->addMaterial(material);


		model_ = new Model();
		if (!model_->load("model/axe.x", shader_))
		{
			return false;
		}

		camera_.lookAt(Vector3(0, 1, -2), Vector3::Zero, Vector3::YAxis);
		setupViewProjMatrix();

		//glEnable(GL_CULL_FACE);
		//glCullFace(GL_BACK);
		glDisable(GL_CULL_FACE);
		return true;
	}

	void setupDynamicUniform()
	{
		shader_->bind();

		Matrix matWorld;
		matWorld.setRotateY(glfwGetTime() * 0.5f);

		bindShaderUniform(shader_.get(), "matWorld", matWorld);
		bindShaderUniform(shader_.get(), "matMVP", matWorld * camera_.getViewProjMatrix());
		bindShaderUniform(shader_.get(), "cameraPos", camera_.getPosition());

		shader_->unbind();
	}

	void onTick() override
	{
		camera_.handleCameraMove();
	}

	void onDraw() override
	{
		Application::onDraw();

		setupDynamicUniform();

		mesh_->draw();

		Matrix matWorld;
		matWorld.setScale(0.1f, 0.1f, 0.1f);
		Renderer::instance()->setViewMatrix(camera_.getViewMatrix());
		Renderer::instance()->setProjMatrix(camera_.getProjMatrix());
		model_->applyMatrix(matWorld);
		model_->draw();
	}

	void onSizeChange(int width, int height) override
	{
		Application::onSizeChange(width, height);
		setupViewProjMatrix();
	}

	void setupViewProjMatrix()
	{
		Vector2 size = getWindowSize();
		camera_.setPerspective(PI_QUARTER, size.x / size.y, 1.0f, 1000.0f);
	}

	virtual void onMouseButton(int button, int action, int mods) override
	{
		camera_.handleMouseButton(button, action, mods);
	}

	virtual void onMouseMove(double x, double y) override
	{
		camera_.handleMouseMove(x, y);
	}

	virtual void onMouseScroll(double xoffset, double yoffset) override
	{
		camera_.handleMouseScroll(xoffset, yoffset);
	}

	ShaderProgramPtr shader_;
	ShaderProgramPtr modelShader_;
	MeshPtr     mesh_;
	Camera		camera_;
	ModelPtr	model_;
};

int main()
{
    MyApplication app;
    if(app.createWindow(800, 600, "013-model-with-assimp"))
    {
        app.mainLoop();
    }
    return 0;
}
