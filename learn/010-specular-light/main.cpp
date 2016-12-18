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
		normalMap->setQuality(TextureQuality::Nearest);

		const char *ShaderFile = "shader/normalmap_specular.shader";
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
		if (un) un->bindValue(normalMap.get());

		Vector3 lightDir(1, 1, 0);
		lightDir.normalize();

		un = shader_->findUniform("lightDir");
		if (un) un->bindValue(lightDir);

		un = shader_->findUniform("lightColor");
		if (un) un->bindValue(Vector3(1.1f));

		un = shader_->findUniform("ambientColor");
		if (un) un->bindValue(Vector3(0.1f));

		un = shader_->findUniform("shininess");
		if (un) un->bindValue(16.0f);

		un = shader_->findUniform("specularStrength");
		if (un) un->bindValue(2.0f);

		mesh_ = createCube(Vector3(1.0f, 1.0f, 1.0f));
		mesh_->addMaterial(shader_);

		camera_.lookAt(Vector3(0, 1, -2), Vector3::Zero, Vector3::YAxis);
		setupViewProjMatrix();

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		return true;
	}

	void setupDynamicUniform()
	{
		shader_->bind();

		Matrix matWorld;
		matWorld.setRotateY(glfwGetTime() * 0.5f);

		ShaderUniform *un;

		un = shader_->findUniform("matWorld");
		if (un) un->bindValue(matWorld);

		un = shader_->findUniform("matMVP");
		if (un) un->bindValue(matWorld * camera_.getViewProjMatrix());

		un = shader_->findUniform("cameraPos");
		if (un) un->bindValue(camera_.getPosition());

		shader_->unbind();
	}

	void onTick() override
	{
		camera_.handleCameraMove();
	}

	void onDraw() override
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
