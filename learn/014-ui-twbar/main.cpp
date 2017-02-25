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

#include <AntTweakBar/AntTweakBar.h>
#include "title.h"

TwType TW_TYPE_VECTOR3;

void getTransformRotate(Vector3 *value, Transform *transfrom)
{
	Vector3 rotation = transfrom->getRotation();
	rotation *= 180.0f / PI_FULL;
	*value = rotation;
}

void setTransformRotate(const Vector3 *value, Transform *transfrom)
{
	Vector3 rotation = *value;
	rotation *= PI_FULL / 180.0f;
	transfrom->setRotation(rotation);
}

class TransformUI
{
public:
	TransformUI()
		: bar_(nullptr)
		, transform_(nullptr)
	{}

	bool init(const std::string &name, Transform *transform)
	{
		if (bar_ != nullptr)
		{
			return false;
		}

		bar_ = TwNewBar(name.c_str());
		transform_ = transform;

#define ADD_VECTOR_VAR(NAME) \
		TwAddVarCB(bar_, #NAME, TW_TYPE_VECTOR3, (TwSetVarCallback)set##NAME, (TwGetVarCallback)get##NAME, transform_, 0)

		ADD_VECTOR_VAR(Position);
		ADD_VECTOR_VAR(Rotation);
		ADD_VECTOR_VAR(Scale);
		return true;
	}

	static void getPosition(Vector3 *value, Transform *transfrom)
	{
		*value = transfrom->getPosition();
	}

	static void setPosition(const Vector3 *value, Transform *transfrom)
	{
		transfrom->setPosition(*value);
	}

	static void getRotation(Vector3 *value, Transform *transfrom)
	{
		Vector3 rotation = transfrom->getRotation();
		rotation *= 180.0f / PI_FULL;
		*value = rotation;
	}

	static void setRotation(const Vector3 *value, Transform *transfrom)
	{
		Vector3 rotation = *value;
		rotation *= PI_FULL / 180.0f;
		transfrom->setRotation(rotation);
	}

	static void getScale(Vector3 *value, Transform *transfrom)
	{
		*value = transfrom->getScale();
	}

	static void setScale(const Vector3 *value, Transform *transfrom)
	{
		transfrom->setScale(*value);
	}

private:
	Transform*  transform_;
	TwBar*		bar_;
};

class MyApplication : public Application
{
public:

	MyApplication()
		: modelUI_(nullptr)
		, cameraUI_(nullptr)
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


		// Initialize AntTweakBar
		TwInit(TW_OPENGL, NULL);

		TwStructMember twVector3[] = {
			{ "x", TW_TYPE_FLOAT, offsetof(Vector3, x), nullptr },
			{ "y", TW_TYPE_FLOAT, offsetof(Vector3, y), nullptr },
			{ "z", TW_TYPE_FLOAT, offsetof(Vector3, z), nullptr },
		};

		TW_TYPE_VECTOR3 = TwDefineStruct("Vector3", twVector3, 3, sizeof(Vector3), 0, 0);

		modelUI_ = new TransformUI();
		modelUI_->init("Model", &modelTransform_);
		TwDefine("Model refresh=0.1");

		cameraUI_ = new TransformUI();
		cameraUI_->init("Camera", &camera_);
		TwDefine("Camera refresh=0.1 position='500 10'");
		return true;
	}

	void onDestroy() override
	{
		delete modelUI_;
		delete cameraUI_;
		TwTerminate();
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

		TwDraw();
	}

	void onSizeChange(int width, int height) override
	{
		Application::onSizeChange(width, height);
		setupViewProjMatrix();

		TwWindowSize(width, height);
	}

	void setupViewProjMatrix()
	{
		Vector2 size = getWindowSize();
		camera_.setPerspective(PI_QUARTER, size.x / size.y, 1.0f, 1000.0f);
	}
	
	virtual void onKey(int key, int scancode, int action, int mods) override
	{
		if (TwEventKeyGLFW(key, action))
			return;
		Application::onKey(key, scancode, action, mods);
	}

	virtual void onMouseButton(int button, int action, int mods) override
	{
		lastCursorPos_ = getCursorPos();
		if (TwEventMouseButtonGLFW(button, action))
			return;
		camera_.handleMouseButton(button, action, mods);
	}

	virtual void onMouseMove(double x, double y) override
	{
		if (TwEventMousePosGLFW(x, y))
			return;

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
		if (TwEventMouseWheelGLFW(yoffset))
			return;
		camera_.handleMouseScroll(xoffset, yoffset);
	}

	virtual void onChar(uint32_t ch) override
	{
		TwEventCharGLFW(ch, 0);
	}

	ShaderProgramPtr modelShader_;
	Camera		camera_;
	ModelPtr	model_;
	Vector2		lastCursorPos_;

	Transform	modelTransform_;
	TransformUI* modelUI_;
	TransformUI* cameraUI_;
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
