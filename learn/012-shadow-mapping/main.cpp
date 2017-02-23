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

		frameSize_.set(1024, 1024);
		camera_.lookAt(Vector3(0, 1, -2), Vector3::Zero, Vector3::YAxis);
		setupViewProjMatrix();

		Vector3 lightDir(2, 1, 0);
		lightDir.normalize();

		mtlOnlyPos_ = new Material();
		if (!mtlOnlyPos_->loadShader("shader/xyz.shader"))
		{
			return false;
		}
		mtlOnlyPos_->setAutoBindUniform(false);

		mtlQuad_ = new Material();
		if (!mtlQuad_->loadShader("shader/xyzuv_upsidedown.shader"))
		{
			return false;
		}
		mtlQuad_->setAutoBindUniform(false);

		mtlShadowMapping_ = new Material();
		if (!mtlShadowMapping_->loadShader("shader/shadowmapping.shader") ||
			!mtlShadowMapping_->loadTexture("u_texture0", "rock.png"))
		{
			return false;
		}

		mtlShadowMapping_->setAutoBindUniform(false);

		mtlShadowMapping_->bindShader();
		mtlShadowMapping_->bindUniform("lightDir", lightDir);
		mtlShadowMapping_->bindUniform("lightColor", Vector3(1.2f));
		mtlShadowMapping_->bindUniform("u_ambientColor", Vector3(0.2f));
		mtlShadowMapping_->bindUniform("shininess", 32.0f);
		mtlShadowMapping_->bindUniform("specularStrength", 4.0f);
		mtlShadowMapping_->bindUniform("texelSize", Vector2(1.0f / frameSize_.x, 1.0f / frameSize_.y));

		frameBuffer_ = new FrameBuffer();
		if (!frameBuffer_->initDepthBuffer(frameSize_.x, frameSize_.y, TextureFormat::Depth))
		{
			LOG_ERROR("Failed create Frame Buffer.");
			return false;
		}

		TexturePtr texture = frameBuffer_->getTexture();
		texture->setQuality(TextureQuality::Nearest);
		texture->setUWrap(TextureWrap::Clamp);
		texture->setVWrap(TextureWrap::Clamp);

		mtlShadowMapping_->setTexture("u_texture1", texture);
		mtlQuad_->setTexture("u_texture0", texture);

		//mesh_ = createCube(Vector3(1.0f, 1.0f, 1.0f));
		mesh_ = createSimpleGround(Vector2(2, 2), 0.5f, 0.1f, 1.0f);
		mesh_->addMaterial(mtlOnlyPos_);

		meshQuad_ = createPlane(Vector2(2, 2), 2);
		meshQuad_->addMaterial(mtlQuad_);

		lightCamera_.lookAt(lightDir * 3.0f, Vector3::Zero, Vector3::YAxis);
		// 注意zfar不要设置太大，否则平均之后的z值就太小了
		lightCamera_.setOrtho(4, 4, 1.0f, 10.0f);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		return true;
	}

	void setupDynamicUniform()
	{
		Matrix matWorld;
		matWorld.setRotateY(glfwGetTime() * 0.5f);

		mtlOnlyPos_->bindShader();
		mtlOnlyPos_->bindUniform("u_matWorldViewProj", matWorld * lightCamera_.getViewProjMatrix());


		mtlShadowMapping_->bindShader();
		mtlShadowMapping_->bindUniform("u_matWorld", matWorld);
		mtlShadowMapping_->bindUniform("u_matWorldViewProj", matWorld * camera_.getViewProjMatrix());
		mtlShadowMapping_->bindUniform("u_cameraPos", camera_.getPosition());
		mtlShadowMapping_->bindUniform("matLightProj", matWorld * lightCamera_.getViewProjMatrix());


		mtlQuad_->bindShader();

		matWorld.setRotateX(-PI_HALF);
		matWorld[3].set(0, 0, 2.0f);
		mtlQuad_->bindUniform("u_matWorldViewProj", matWorld * camera_.getViewProjMatrix());
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
			
			mesh_->setMaterial(0, mtlOnlyPos_);
			mesh_->draw();

			frameBuffer_->unbind();

			// 恢复视口
			Vector2 size = getWindowSize();
			glViewport(0, 0, size.x, size.y);
		}

		glClearColor(0.15f, 0.24f, 0.24f, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		mesh_->setMaterial(0, mtlShadowMapping_);
		mesh_->draw();

		meshQuad_->draw();
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
		//camera_.setOrtho(4, 4, 1.0f, 1000.0f);
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

	MaterialPtr mtlOnlyPos_;
	MaterialPtr mtlQuad_;
	MaterialPtr mtlShadowMapping_;

	MeshPtr     mesh_;
	Camera		camera_;

	FrameBufferPtr frameBuffer_;
	MeshPtr		meshQuad_;
	Vector2		frameSize_;

	// 站在灯光的位置观察世界
	Camera		lightCamera_;
};

int main()
{
    MyApplication app;
    if(app.createWindow(800, 600, "012-shadow-mapping"))
    {
        app.mainLoop();
    }
    return 0;
}
