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

        lightTransform_ = new Transform();
        lightTransform_->setRotation(45.0f, 0.0f, 0.0f);

        ground_ = new Transform();
        MeshPtr groundMesh = createPlane(Vector2(10, 10), 1);
        ground_->addComponent(groundMesh);

        casters_ = new Transform();

        MeshPtr cubeMesh = createCube(Vector3(1, 1, 1));
        material_ = new Material();
		if (!material_->loadShader("shader/light_pixel.shader"))
		{
			return false;
		}
        material_->loadTexture("u_texture0", "white.png");

        material_->bindShader();
        material_->bindUniform("lightColor", Color(1.0f, 1.0f, 1.0f, 1.0f));

        cubeMesh->addMaterial(material_);
        groundMesh->addMaterial(material_);

        Vector3 positions[] = {
            { -1, 0.5f, -4 },
            { -1, 0.5f, -2 },
            { -1, 0.5f, 0},
            { -1, 0.5f, 2},
            { -1, 0.5f, 4},
            { 1, 0.5f, -4 },
            { 1, 0.5f, -2 },
            { 1, 0.5f, 0 },
            { 1, 0.5f, 2 },
            { 1, 0.5f, 4 },
        };
        for (const Vector3 &pos : positions)
        {
            TransformPtr t = new Transform();
            t->setPosition(pos);
            t->addComponent(cubeMesh);
            casters_->addChild(t);
        }

		camera_.lookAt(Vector3(0, 2, -6), Vector3::Zero, Vector3::YAxis);
		setupViewProjMatrix();
		Renderer::instance()->setCamera(&camera_);

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
		Application::onDraw(renderer);

        material_->bindShader();
        material_->bindUniform("lightDir", -lightTransform_->getForwardVector());

        ground_->draw(renderer);
        casters_->draw(renderer);
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

			Vector3 rotation = lightTransform_->getRotation();
			rotation.y -= dx * PI_FULL;
			rotation.x -= dy * PI_FULL;
            lightTransform_->setRotation(rotation);
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

    MaterialPtr material_;

    TransformPtr    casters_;
    TransformPtr    ground_;
    TransformPtr    lightTransform_;
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
