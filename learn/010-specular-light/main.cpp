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
		TexturePtr normalMap = TextureMgr::instance()->get("rock-normal.png");
		if (!texture || !normalMap)
		{
			return false;
		}
		texture->setQuality(TextureQuality::TwoLinear);
		normalMap->setQuality(TextureQuality::ThreeLinear);

		const char *ShaderFile = "shader/normalmap_specular.shader";
		shader_ = ShaderProgramMgr::instance()->get(ShaderFile);
		if (!shader_)
		{
			return false;
		}

		Vector3 lightDir(1, 1, 0);
		lightDir.normalize();

		shader_->bind();
		bindShaderUniform(shader_.get(), "u_texture0", texture.get());
		bindShaderUniform(shader_.get(), "u_texture1", normalMap.get());
		bindShaderUniform(shader_.get(), "lightDir", lightDir);
		bindShaderUniform(shader_.get(), "lightColor", Vector3(1.5f));
		bindShaderUniform(shader_.get(), "ambientColor", Vector3(0.5f));
		bindShaderUniform(shader_.get(), "shininess", 64.0f);
		bindShaderUniform(shader_.get(), "specularStrength", 4.0f);
		shader_->unbind();

		mesh_ = createCube(Vector3(1.0f, 1.0f, 1.0f));
		mesh_->addMaterial(shader_);

		camera_.lookAt(Vector3(0, 1, -2), Vector3::Zero, Vector3::YAxis);
		setupViewProjMatrix();

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
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
	MeshPtr     mesh_;
	Camera		camera_;
};

int main()
{
    MyApplication app;
    if(app.createWindow(800, 600, "010-specular-light"))
    {
        app.mainLoop();
    }
    return 0;
}
