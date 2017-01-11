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

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

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

		TexturePtr texture = TextureMgr::instance()->get("rock.png");
		TexturePtr normalMap = TextureMgr::instance()->get("rock-normal.png");
		if (!texture || !normalMap)
		{
			return false;
		}
		texture->setQuality(TextureQuality::TwoLinear);
		normalMap->setQuality(TextureQuality::ThreeLinear);

		const char *ShaderFile = "shader/light_pixel.shader";
		shader_ = ShaderProgramMgr::instance()->get(ShaderFile);
		if (!shader_)
		{
			return false;
		}

		Vector3 lightDir(1, 1, 0);
		lightDir.normalize();

		modelShader_->bind();
		bindShaderUniform(modelShader_.get(), "lightDir", lightDir);
		bindShaderUniform(modelShader_.get(), "lightColor", Vector3(1.5f));
		bindShaderUniform(modelShader_.get(), "ambientColor", Vector3(0.5f));
		modelShader_->unbind();

		shader_->bind();
		bindShaderUniform(shader_.get(), "lightDir", lightDir);
		bindShaderUniform(shader_.get(), "lightColor", Vector3(1.5f));
		bindShaderUniform(shader_.get(), "ambientColor", Vector3(0.5f));
		bindShaderUniform(shader_.get(), "shininess", 64.0f);
		bindShaderUniform(shader_.get(), "specularStrength", 4.0f);
		shader_->unbind();

		mesh_ = createCube(Vector3(1.0f, 1.0f, 1.0f));
		MaterialPtr material = new Material();
		material->setShader(shader_);
		material->setTexture("u_texture0", texture.get());
		material->setTexture("u_texture1", normalMap.get());
		mesh_->addMaterial(material);


		model_ = new Model();
		if (!model_->load("model/axe.x", modelShader_))
		{
			return false;
		}

		camera_.lookAt(Vector3(0, 1, -2), Vector3::Zero, Vector3::YAxis);
		setupViewProjMatrix();

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		//glDisable(GL_CULL_FACE);


		// Initialize AntTweakBar
		TwInit(TW_OPENGL, NULL);

		// Create a tweak bar
		bar = TwNewBar("TweakBar");
		TwDefine(" GLOBAL help='This example shows how to integrate AntTweakBar with GLFW and OpenGL.' "); // Message added to the help bar.

																										   // Add 'speed' to 'bar': it is a modifable (RW) variable of type TW_TYPE_DOUBLE. Its key shortcuts are [s] and [S].
		static double time = 0, dt;// Current time and enlapsed time
		static double turn = 0;    // Model turn counter
		static double speed = 0.3; // Model rotation speed
		static int wire = 0;       // Draw model in wireframe?
		static float bgColor[] = { 0.1f, 0.2f, 0.4f };         // Background color 
		static unsigned char cubeColor[] = { 255, 0, 0, 128 }; // Model color (32bits RGBA)

		TwAddVarRW(bar, "speed", TW_TYPE_DOUBLE, &speed,
			" label='Rot speed' min=0 max=2 step=0.01 keyIncr=s keyDecr=S help='Rotation speed (turns/second)' ");

		// Add 'wire' to 'bar': it is a modifable variable of type TW_TYPE_BOOL32 (32 bits boolean). Its key shortcut is [w].
		
		TwAddVarRW(bar, "wire", TW_TYPE_BOOL32, &wire,
			" label='Wireframe mode' key=w help='Toggle wireframe display mode.' ");

		// Add 'time' to 'bar': it is a read-only (RO) variable of type TW_TYPE_DOUBLE, with 1 precision digit
		TwAddVarRO(bar, "time", TW_TYPE_DOUBLE, &time, " label='Time' precision=1 help='Time (in seconds).' ");

		// Add 'bgColor' to 'bar': it is a modifable variable of type TW_TYPE_COLOR3F (3 floats color)
		TwAddVarRW(bar, "bgColor", TW_TYPE_COLOR3F, &bgColor, " label='Background color' ");

		// Add 'cubeColor' to 'bar': it is a modifable variable of type TW_TYPE_COLOR32 (32 bits color) with alpha
		TwAddVarRW(bar, "cubeColor", TW_TYPE_COLOR32, &cubeColor,
			" label='Cube color' alpha help='Color and transparency of the cube.' ");
		return true;
	}

	void onDestroy() override
	{
		TwTerminate();
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

		//mesh_->draw();

		Matrix matWorld;
		matWorld.setIdentity();
		matWorld.setScale(0.1f, 0.1f, 0.1f);
		Renderer::instance()->setViewMatrix(camera_.getViewMatrix());
		Renderer::instance()->setProjMatrix(camera_.getProjMatrix());
		model_->applyMatrix(matWorld);
		model_->draw();

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
		Application::onKey(key, scancode, action, mods);
		TwEventKeyGLFW(key, action);
	}

	virtual void onMouseButton(int button, int action, int mods) override
	{
		camera_.handleMouseButton(button, action, mods);
		TwEventMouseButtonGLFW(button, action);
	}

	virtual void onMouseMove(double x, double y) override
	{
		camera_.handleMouseMove(x, y);
		TwEventMousePosGLFW(x, y);
	}

	virtual void onMouseScroll(double xoffset, double yoffset) override
	{
		camera_.handleMouseScroll(xoffset, yoffset);
		TwEventMouseWheelGLFW(yoffset);
	}

	virtual void onChar(uint32_t ch) override
	{
		TwEventCharGLFW(ch, 0);
	}

	ShaderProgramPtr shader_;
	ShaderProgramPtr modelShader_;
	MeshPtr     mesh_;
	Camera		camera_;
	ModelPtr	model_;
	TwBar *bar;         // Pointer to a tweak bar
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
