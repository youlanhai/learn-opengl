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
#include "title.h"

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

		Texture::s_defaultQuality = TextureQuality::FourLinear;

		Vector3 lightDir(1, 1, -1);
		lightDir.normalize();

		cubeMesh_ = createCube(Vector3(1, 1, 1));
		skyMesh_ = createCube(Vector3(50));

		MaterialPtr cubeMaterial = new Material();
		if (!cubeMaterial->loadShader("shader/normalmap_diffuse.shader"))
		{
			return false;
		}
		cubeMaterial->loadTexture("u_texture0", "rock.png");
		cubeMaterial->loadTexture("u_texture1", "rock-normal.png");

		cubeMaterial->bindShader();
		cubeMaterial->bindUniform("lightDir", lightDir);
		cubeMaterial->bindUniform("lightColor", Vector3(1.5f));

		MaterialPtr skyMaterial = new Material();
		if (!skyMaterial->loadShader("shader/cube_map.shader"))
		{
			return false;
		}
		skyMaterial->loadTexture("u_texture0", "ame_ash/ash.cube");

		cubeMesh_->addMaterial(cubeMaterial);
		skyMesh_->addMaterial(skyMaterial);

		camera_.lookAt(Vector3(0, 1, -2), Vector3::Zero, Vector3::YAxis);
		setupViewProjMatrix();
		Renderer::instance()->setCamera(&camera_);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		return true;
	}

	void onTick() override
	{
		camera_.handleCameraMove();
	}

	void onDraw() override
	{
		Application::onDraw();

		auto renderer = Renderer::instance();
		renderer->applyCameraMatrix();

		glCullFace(GL_FRONT);
		renderer->setWorldMatrix(Matrix::Identity);
		skyMesh_->draw();

		glCullFace(GL_BACK);
		renderer->setWorldMatrix(modelTransform_.getModelMatrix());
		cubeMesh_->draw();
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
		lastCursorPos_ = getCursorPos();
		camera_.handleMouseButton(button, action, mods);
	}

	virtual void onMouseMove(double x, double y) override
	{
		if (gApp->isMousePress(GLFW_MOUSE_BUTTON_LEFT))
		{
			Vector2 size = gApp->getWindowSize();

			float dx = (float(x) - lastCursorPos_.x) / size.x;
			float dy = (float(y) - lastCursorPos_.y) / size.y;

			Vector3 rotation = modelTransform_.getRotation();
			rotation.y -= dx * PI_FULL;
			rotation.x -= dy * PI_FULL;
			modelTransform_.setRotation(rotation);
		}
		else
		{
			camera_.handleMouseMove(x, y);
		}

		lastCursorPos_.set(x, y);
	}

	virtual void onMouseScroll(double xoffset, double yoffset) override
	{
		camera_.handleMouseScroll(xoffset, yoffset);
	}

	Camera		camera_;
	Vector2		lastCursorPos_;
	Transform	modelTransform_;

	MeshPtr		cubeMesh_;
	MeshPtr		skyMesh_;
};

int main()
{
    MyApplication app;
    if(app.createWindow(800, 600, APP_TITLE))
    {
        app.mainLoop();
    }
    return 0;
}
