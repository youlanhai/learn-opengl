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

        MeshPtr cubeMesh = createCube(Vector3(1, 1, 1));
        MeshPtr skyMesh = createCube(Vector3(50));

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

        TexturePtr cubeTexture = TextureMgr::instance()->get("skybox/skybox.cube");

        MaterialPtr reflectMaterial = new Material();
        if (!reflectMaterial->loadShader("shader/cube_reflect.shader"))
        {
            return false;
        }
        reflectMaterial->setTexture("u_texture0", cubeTexture);

        MaterialPtr refractMaterial = new Material();
        if (!refractMaterial->loadShader("shader/cube_refract.shader"))
        {
            return false;
        }
        refractMaterial->setTexture("u_texture0", cubeTexture);

        materials_[0] = cubeMaterial;
        materials_[1] = reflectMaterial;
        materials_[2] = refractMaterial;

		MaterialPtr skyMaterial = new Material();
		if (!skyMaterial->loadShader("shader/cube_map.shader"))
		{
			return false;
		}
		skyMaterial->setTexture("u_texture0", cubeTexture);

		cubeMesh->addMaterial(materials_[2]);
		skyMesh->addMaterial(skyMaterial);

        modelTransform_.addComponent(cubeMesh);
        skyTransform_.addComponent(skyMesh);

		camera_.lookAt(Vector3(0, 0, -2), Vector3::Zero, Vector3::YAxis);
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

		glCullFace(GL_FRONT);
        skyTransform_.draw(renderer);
		
        glCullFace(GL_BACK);
        modelTransform_.draw(renderer);
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

    virtual void onKey(int key, int scancode, int action, int mods) override
    {
        Application::onKey(key, scancode, action, mods);

        if (action == GLFW_RELEASE)
        {
            int i = -1;
            switch(key)
            {
            case GLFW_KEY_1: i = 0; break;
            case GLFW_KEY_2: i = 1; break;
            case GLFW_KEY_3: i = 2; break;
            }

            if (i >= 0)
            {
                MeshPtr mesh = modelTransform_.getComponentByType(typeid(Mesh));
                if (mesh)
                {
                    mesh->setMaterial(0, materials_[i]);
                }
            }
        }
    }

	Camera		camera_;
	Vector2		lastCursorPos_;

	Transform	modelTransform_;
    Transform   skyTransform_;

    MaterialPtr materials_[3];
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
