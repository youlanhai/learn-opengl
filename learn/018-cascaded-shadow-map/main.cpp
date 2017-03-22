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

        Texture::s_defaultQuality = TextureQuality::FourLinear;

        ground_ = new Transform();
        MeshPtr groundMesh = createPlane(Vector2(10, 10), 1);
        ground_->addComponent(groundMesh);

        casters_ = new Transform();

        MeshPtr cubeMesh = createCube(Vector3(1, 1, 1));
        material_ = new Material();
        if (!material_->loadShader("shader/cascade_shadowmap.shader"))
        {
            return false;
        }
        material_->loadTexture("u_texture0", "white.png");

        material_->bindShader();
        material_->bindUniform("lightColor", Color(1.0f, 1.0f, 1.0f, 1.0f));

        lightMaterial_ = new Material();
        if (!lightMaterial_->loadShader("shader/xyz.shader"))
        {
            return false;
        }

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
            { -3, 0.5f, 4 },
            { 3, 0.5f, 4 },
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

        lightCamera_.setPosition(0, 6, -6);
        lightCamera_.setRotation(PI_FULL / 6.0f, PI_FULL / 3.0f, 0.0f);
        lightCamera_.setOrtho(20, 20, 0, 10);

        frameBuffer_ = new FrameBuffer();
        frameBuffer_->initDepthBuffer(1024, 1024, TextureFormat::Depth);
        TexturePtr texture = frameBuffer_->getTexture();
        texture->setQuality(TextureQuality::Nearest);
        texture->setUWrap(TextureWrap::Clamp);
        texture->setVWrap(TextureWrap::Clamp);

        //material_->setTexture("u_texture0", texture);
        material_->setTexture("u_texture1", texture);

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
        renderer->setCamera(&lightCamera_);
        renderer->setOverwriteMaterial(lightMaterial_);

        frameBuffer_->bind();
        Vector2 size = frameBuffer_->getSize();
        glViewport(0, 0, size.x, size.y);
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //ground_->draw(renderer);
        casters_->draw(renderer);

        frameBuffer_->unbind();
        renderer->setOverwriteMaterial(nullptr);


        size = getFrameBufferSize();
        glViewport(0, 0, size.x, size.y);
        glClearColor(0.15f, 0.24f, 0.24f, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderer->setCamera(&camera_);
        material_->bindShader();
        material_->bindUniform("lightDir", -lightCamera_.getForwardVector());
        material_->bindUniform("lightMatrix", lightCamera_.getViewProjMatrix());

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
		Vector2 size = getFrameBufferSize();
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

			Vector3 rotation = lightCamera_.getRotation();
			rotation.y -= dx * PI_FULL;
			rotation.x -= dy * PI_FULL;
            lightCamera_.setRotation(rotation);
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

    Camera          lightCamera_;
    MaterialPtr     lightMaterial_;

    FrameBufferPtr  frameBuffer_;
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
