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

		Vector3 lightDir(1, 1, 0);
		lightDir.normalize();

		modelShader_->bind();
		bindShaderUniform(modelShader_.get(), "lightDir", lightDir);
		bindShaderUniform(modelShader_.get(), "lightColor", Vector3(1.5f));
		bindShaderUniform(modelShader_.get(), "u_ambientColor", Vector3(0.5f));
		modelShader_->unbind();

		model_ = new Model();
		if (!model_->load("model/axe.x", modelShader_))
		{
			return false;
		}
		modelTransform_.setScale(0.1f);

		camera_.lookAt(Vector3(0, 1, -2), Vector3::Zero, Vector3::YAxis);
		setupViewProjMatrix();

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		//glDisable(GL_CULL_FACE);

		return true;
	}

	void onTick(float elapse) override
	{
		camera_.handleCameraMove();
	}

	void onDraw(Renderer *renderer) override
	{
		Application::onDraw(renderer);

        renderer->setViewMatrix(camera_.getViewMatrix());
        renderer->setProjMatrix(camera_.getProjMatrix());
        renderer->setWorldMatrix(modelTransform_.getModelMatrix());
		model_->draw(renderer);
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

	ShaderProgramPtr modelShader_;
	Camera		camera_;
	ModelPtr	model_;
	Vector2		lastCursorPos_;
	Transform	modelTransform_;
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
