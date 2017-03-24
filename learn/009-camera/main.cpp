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
		if (!texture)
		{
			return false;
		}
		texture->setQuality(TextureQuality::ThreeLinear);

		const char *ShaderFile = "shader/light_pixel.shader";
		shader_ = ShaderProgramMgr::instance()->get(ShaderFile);
		if (!shader_)
		{
			return false;
		}

		shader_->bind();
		ShaderUniform *un;

		un = shader_->findUniform("u_texture0");
		if (un) un->bindValue(texture.get());

		Vector3 lightDir(1, 1, 0);
		lightDir.normalize();

		un = shader_->findUniform("lightDir");
		if (un) un->bindValue(lightDir);

		un = shader_->findUniform("lightColor");
		if (un) un->bindValue(Vector3(1.0f));

		un = shader_->findUniform("u_ambientColor");
		if (un) un->bindValue(Vector3(0.3f));

		mesh_ = createCube(Vector3(1.0f, 1.0f, 1.0f));
		MaterialPtr material = new Material();
		material->setShader(shader_);
		material->setAutoBindUniform(false);
		mesh_->addMaterial(material);

		camera_.lookAt(Vector3(0, 1, -2), Vector3::Zero, Vector3::YAxis);
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
		if (un) un->bindValue(matWorld * camera_.getViewProjMatrix());

		shader_->unbind();
	}

	void onTick(float elapse) override
	{
		camera_.handleCameraMove();
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
		camera_.setPerspective(PI_QUARTER, getAspect(), 1.0f, 1000.0f);
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
    if(app.createWindow(800, 600, "009-camera"))
    {
        app.mainLoop();
    }
    return 0;
}
