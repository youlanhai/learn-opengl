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
#include "FrameBuffer.h"

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

		Texture::s_defaultQuality = TextureQuality::TwoLinear;

		camera_.lookAt(Vector3(0, 1, -2), Vector3::Zero, Vector3::YAxis);
		setupViewProjMatrix();

		Vector3 lightDir(1, 1, 0);
		lightDir.normalize();

		material1_= new Material();
		if (!material1_->loadShader("shader/normalmap_specular.shader") ||
			!material1_->loadTexture("u_texture0", "rock.png") ||
			!material1_->loadTexture("u_texture1", "rock-normal.png"))
		{
			return false;
		}
		material1_->setAutoBindUniform(false);

		material1_->bindShader();
		material1_->bindUniform("lightDir", lightDir);
		material1_->bindUniform("lightColor", Vector3(1.5f));
		material1_->bindUniform("u_ambientColor", Vector3(0.5f));
		material1_->bindUniform("shininess", 64.0f);
		material1_->bindUniform("specularStrength", 4.0f);

		mesh_ = createCube(Vector3(1.0f, 1.0f, 1.0f));
		mesh_->addMaterial(material1_);

		material2_ = new Material();
		if(!material2_->loadShader("shader/xyzuv_upsidedown.shader"))
		{
			return false;
		}
		material2_->setAutoBindUniform(false);

		frameSize_.set(1024, 1024);
#if 1 
		frameBuffer_ = new FrameBuffer();
		if (!frameBuffer_->initColorBuffer(frameSize_.x, frameSize_.y, TextureFormat::RGBA))
		//if (!frameBuffer_->initDepthBuffer(frameSize_.x, frameSize_.y, TextureFormat::Depth))
		{
			LOG_ERROR("Failed create Frame Buffer.");
			return false;
		}

		TexturePtr texture = frameBuffer_->getTexture();
		texture->setQuality(TextureQuality::Nearest);
		texture->setUWrap(TextureWrap::Repeat);
		texture->setVWrap(TextureWrap::Repeat);
		material2_->setTexture("u_texture0", texture);

		mesh2_ = createPlane(Vector2(2, 2), 2);
		mesh2_->addMaterial(material2_);
#endif

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		return true;
	}

	void setupDynamicUniform()
	{
		material1_->bindShader();

		Matrix matWorld;
		matWorld.setRotateY(glfwGetTime() * 0.5f);

		material1_->bindUniform("u_matWorld", matWorld);
		material1_->bindUniform("u_matWorldViewProj", matWorld * camera_.getViewProjMatrix());
		material1_->bindUniform("u_cameraPos", camera_.getPosition());


		material2_->bindShader();

		matWorld.setRotateX(-PI_HALF);
		Matrix mat2;
		mat2.setTranslate(0, 0, 2.0f);
		matWorld.postMultiply(mat2);
		material2_->bindUniform("u_matWorldViewProj", matWorld * camera_.getViewProjMatrix());
	}

	void onTick() override
	{
		camera_.handleCameraMove();
	}

	void onDraw() override
	{
		setupDynamicUniform();

		if (frameBuffer_)
		{
			frameBuffer_->bind();

			glViewport(0, 0, frameSize_.x, frameSize_.y);
			glClearColor(0, 0, 0, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			
			mesh_->draw();

			frameBuffer_->unbind();

			// 恢复视口
			Vector2 size = getWindowSize();
			glViewport(0, 0, size.x, size.y);
		}

		glClearColor(0.15f, 0.24f, 0.24f, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		mesh_->draw();
		mesh2_->draw();
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

	MaterialPtr material1_;
	MeshPtr     mesh_;
	Camera		camera_;

	FrameBufferPtr frameBuffer_;
	MeshPtr		mesh2_;
	MaterialPtr material2_;
	Vector2		frameSize_;
};

int main()
{
    MyApplication app;
    if(app.createWindow(800, 600, "011-render-to-texture"))
    {
        app.mainLoop();
    }
    return 0;
}
