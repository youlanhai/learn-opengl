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
#if TEST_SHADOW_VOLUME
		if (!materialVolume_->loadShader("shader/xyzcolor.shader"))
#else
		if (!materialVolume_->loadShader("shader/xyz.shader"))
#endif
		{
			return false;
		}

		mesh1_ = createPlane(Vector2(10.0f, 10.0f), 1.0f);
		mesh1_->addMaterial(material_);

		mesh2_ = createCube(Vector3(1.0f, 1.0f, 1.0f));
		mesh2_->addMaterial(material_);

		modelTransform_.setScale(0.1f);

		camera_.lookAt(Vector3(0, 2, -4), Vector3::Zero, Vector3::YAxis);
		setupViewProjMatrix();

		Renderer::instance()->setCamera(&camera_);

		glDisable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		// 开启模板测试
		//glEnable(GL_STENCIL_TEST);
		return true;
	}

	void onTick() override
	{
		camera_.handleCameraMove();
	}

	void onDraw() override
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		auto renderer = Renderer::instance();
		renderer->applyCameraMatrix();

		lightDir_ = modelTransform_.getForwardVector();
		lightDir_.normalize();

		material_->bindShader();
		material_->bindUniform("lightDir", -lightDir_);

#if 0
		// 把矩形覆盖的区域模版值增加为 1
		glStencilFunc(GL_ALWAYS, 0, 0xffffffff);
		glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);

		// 关闭颜色写入和深度写入。目的是只改变模板的值，而不影响原来的深度值和颜色值。
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glDepthMask(GL_FALSE);
		drawQuad();

		// 恢复颜色写入和深度写入
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glDepthMask(GL_TRUE);

		// 只允许掩码值(0) < 模板值的区域可以进行渲染
		glStencilFunc(GL_LESS, 0, 0xffffffff);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
#endif

		renderScene();
	}

	void renderScene()
	{
		auto renderer = Renderer::instance();

		Matrix matWorld;
		matWorld.setIdentity();

		renderer->setWorldMatrix(matWorld);
		//mesh1_->draw();

		matWorld.setTranslate(-1.0f, 1.0f, 0.0f);
		renderer->setWorldMatrix(matWorld);
		mesh2_->draw();

		matWorld.setTranslate(1.0f, 1.0f, 0.0f);
		renderer->setWorldMatrix(matWorld);
		//mesh2_->draw();
		MeshPtr volume = createShaowVolumeForDirectionLight(mesh2_, matWorld, lightDir_);
		if (volume)
		{
			volume->addMaterial(materialVolume_);

			glDisable(GL_CULL_FACE);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			renderer->setWorldMatrix(Matrix::Identity);
			volume->draw();
			glDisable(GL_BLEND);
			glEnable(GL_CULL_FACE);
		}
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

	MeshPtr		mesh1_;
	MeshPtr		mesh2_;

	MaterialPtr material_;
	Vector3		lightDir_;
	MaterialPtr  materialVolume_;
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
