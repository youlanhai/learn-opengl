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
#include "Vertex.h"
#include "VertexBuffer.h"
#include "Material.h"


class MyApplication : public Application
{
public:

	MyApplication()
		: lightDir_(1, 1, 0)
		, showVolume_(false)
		, showShadow_(true)
		, showCaster_(true)
	{
		lightDir_.normalize();
		glfwWindowHint(GLFW_SAMPLES, 4);
	}

	bool onCreate() override
	{
		std::string resPath = findResPath();
		FileSystem::instance()->addSearchPath(resPath);
		FileSystem::instance()->addSearchPath(joinPath(resPath, "common"));
		FileSystem::instance()->dumpSearchPath();

		Texture::s_defaultQuality = TextureQuality::ThreeLinear;

		Vector3 lightColor(1.5f);

		material_ = new Material();
		if(!material_->loadShader("shader/normalmap_specular.shader"))
		{ 
			return false;
		}
		material_->loadTexture("u_texture0", "rock.png");
		material_->loadTexture("u_texture1", "rock-normal.png");

		material_->bindShader();
		material_->bindUniform("lightDir", lightDir_);
		material_->bindUniform("lightColor", lightColor);
		material_->bindUniform("shininess", 32.0f);
		material_->bindUniform("specularStrength", 2.0f);

		materialVolume_ = new Material();
		if (!materialVolume_->loadShader("shader/xyz_ucolor.shader"))
		{
			return false;
		}

		meshPlane_ = createPlane(Vector2(10.0f, 10.0f), 1.0f);
		meshPlane_->addMaterial(material_);

		meshCube_ = createCube(Vector3(1.0f, 1.0f, 1.0f));
		meshCube_->addMaterial(material_);

		meshQuad_ = createQuad(Vector2(2, 2));
		meshQuad_->addMaterial(materialVolume_);

		camera_.lookAt(Vector3(0, 2, -4), Vector3::Zero, Vector3::YAxis);
		camera_.setMoveSpeed(3.0f);
		setupViewProjMatrix();
		Renderer::instance()->setCamera(&camera_);

		lightTransform_.lookAt(Vector3(1, 1, -0.5), Vector3::Zero, Vector3::YAxis);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		return true;
	}

	void onTick(float elapse) override
	{
		camera_.handleCameraMove();
	}

	void onDraw(Renderer *renderer) override
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		lightDir_ = lightTransform_.getForwardVector();
		lightDir_.normalize();

		material_->bindShader();
		material_->bindUniform("lightDir", -lightDir_);

		renderScene();
		renderShadow();
	}

	void renderScene()
	{
		auto renderer = Renderer::instance();

		Matrix matWorld;
		matWorld.setIdentity();

		renderer->setWorldMatrix(matWorld);
		meshPlane_->draw(renderer);

		matWorld.setTranslate(-1.0f, 1.0f, 0.0f);
		renderer->setWorldMatrix(matWorld);
		meshCube_->draw(renderer);

		matWorld.setTranslate(1.0f, 1.0f, 0.0f);
		renderer->setWorldMatrix(matWorld);
		if (showCaster_)
		{
			meshCube_->draw(renderer);
		}
	}

	void renderShadow()
	{
		auto renderer = Renderer::instance();

		Matrix matWorld = renderer->getWorldMatrix();
		MeshPtr volume = createShaowVolumeForDirectionLight(meshCube_, matWorld, lightDir_);
		if (!volume)
		{
			return;
		}

		volume->addMaterial(materialVolume_);
		renderer->setWorldMatrix(Matrix::Identity);

		// ZFail 算法
		glEnable(GL_STENCIL_TEST);
		glStencilFunc(GL_ALWAYS, 0, 0xffffffff);
		renderer->setColorWriteEnable(false);
		renderer->setZWriteEnable(false);

		// 先渲染背面。深度测试失败时，模板值+1
		glCullFace(GL_FRONT);
		glStencilOp(GL_KEEP, GL_INCR, GL_KEEP);
		volume->draw(renderer);

		// 再渲染正面。深度测试是吧时，模板值-1
		glCullFace(GL_BACK);
		glStencilOp(GL_KEEP, GL_DECR, GL_KEEP);
		volume->draw(renderer);

		renderer->setColorWriteEnable(true);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

		// 显示阴影体
		if (showVolume_)
		{
			materialVolume_->bindShader();
			materialVolume_->bindUniform("u_color", Color(0.5f, 0.5f, 0.5f, 0.5f));

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			volume->draw(renderer);
			glDisable(GL_BLEND);
		}

		renderer->setZWriteEnable(true);

		if (showShadow_)
		{
			// 然后渲染一个全屏的黑色图
			materialVolume_->bindShader();
			materialVolume_->bindUniform("u_color", Color(0.0f, 0.0f, 0.0f, 0.8f));
			renderer->setViewMatrix(Matrix::Identity);
			renderer->setProjMatrix(Matrix::Identity);

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glDisable(GL_DEPTH_TEST);
			glStencilFunc(GL_LESS, 0, 0xffffffff);
			meshQuad_->draw(renderer);

			glDisable(GL_BLEND);
			glEnable(GL_DEPTH_TEST);
		}

		glDisable(GL_STENCIL_TEST);
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
	
	virtual void onKey(int key, int scancode, int action, int mods) override
	{
		Application::onKey(key, scancode, action, mods);

		if (action == GLFW_RELEASE)
		{
			switch(key)
			{
			case GLFW_KEY_1:
				showShadow_ = !showShadow_;
				break;
			case GLFW_KEY_2:
				showVolume_ = !showVolume_;
				break;
			case GLFW_KEY_3:
				showCaster_ = !showCaster_;
				break;
			default:
				break;
			}
		}
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

			Vector3 rotation = lightTransform_.getRotation();
			rotation.y -= dx * PI_FULL;
			rotation.x += dy * PI_FULL;
			lightTransform_.setRotation(rotation);
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
	Transform	lightTransform_;

	MeshPtr		meshPlane_;
	MeshPtr		meshCube_;
	MeshPtr		meshQuad_;

	MaterialPtr material_;
	Vector3		lightDir_;
	MaterialPtr  materialVolume_;

	bool		showVolume_;
	bool		showShadow_;
	bool		showCaster_;
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
