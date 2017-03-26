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
#include "Texture2DArray.h"
#include "Ray.h"
#include "MeshFaceVisitor.h"


class MyApplication : public Application
{
public:

	MyApplication()
	{
		glfwWindowHint(GLFW_SAMPLES, 4);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	}

    bool onCreate() override
    {
        std::string resPath = findResPath();
        FileSystem::instance()->addSearchPath(resPath);
        FileSystem::instance()->addSearchPath(joinPath(resPath, "common"));
        FileSystem::instance()->dumpSearchPath();

        Texture::s_defaultQuality = TextureQuality::FourLinear;
        
        Vector2 size = getFrameBufferSize();
        winWidth_ = size.x;
        winHeight_ = size.y;
        pixels_.resize(winWidth_ * winHeight_ * 4);
        
        texture_ = new Texture();
        texture_->create(0, winWidth_, winHeight_, TextureFormat::RGBA, nullptr, GL_UNSIGNED_BYTE);

        if(!createMaterials())
        {
            return false;
        }
        
        MaterialPtr material = getCurrentMaterial();
 
        cubeMesh_ = createCube(Vector3(1, 1, 1));
        cubeMesh_->addMaterial(material);
        cubeMesh_->generateBoundingBox();

        objects_ = new Transform();
        Vector4 positions[] = {
            {0, -5, 0, 10}, // bottom
            {0, 15, 0, 10}, // top
            {-10, 5, 0, 10}, // left
            {10, 5, 0, 10}, // right
            {0, 5, 10, 10}, // back
            {0, 0.5f, 0, 1},
            {0, 1.0f + 0.25f, 0, 0.5f},
            {-1.5f, 0.5f, 0, 1},
            {1.8f, 0.5f, 0, 1},
        };
        for (const Vector4 &pos4 : positions)
        {
            TransformPtr t = new Transform();
            t->setPosition(Vector3(pos4));
            t->setScale(pos4.w);
            t->addComponent(cubeMesh_);
            objects_->addChild(t);
        }

		camera_.lookAt(Vector3(0, 3, -10), Vector3::Zero, Vector3::YAxis);
		setupProjectionMatrix();
		Renderer::instance()->setCamera(&camera_);

        lightCamera_.setPosition(0, 6, -6);
        lightCamera_.setRotation(PI_FULL / 6.0f, PI_FULL / 3.0f, 0.0f);
        lightCamera_.setOrtho(20, 20, 1, 10);

        
        quadMesh_ = createQuad(Vector2(0.4f, 0.4f));
        quadMesh_->addMaterial(quadMaterial_);
        
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		return true;
	}
    
    bool createMaterials()
    {
        MaterialPtr material;
        
        material = new Material();
        materials_[0] = material;
        if(!material->loadShader("shader/light_pixel.shader"))
        {
            return false;
        }
        
        material = new Material();
        materials_[1] = material;
        if (!material->loadShader("shader/light_vertex.shader"))
        {
            return false;
        }
        
        for(MaterialPtr &mtl : materials_)
        {
            mtl->bindShader();
            mtl->loadTexture("u_texture0", "white.png");
            mtl->bindUniform("lightColor", Color(1.0f, 1.0f, 1.0f, 1.0f));
        }
        
        quadMaterial_ = new Material();
        if(!quadMaterial_->loadShader("shader/xyzuv.shader"))
        {
            return false;
        }
        quadMaterial_->setTexture("u_texture0", texture_);
        return true;
    }
    
	void onTick(float elapse) override
	{
		camera_.handleCameraMove();
	}

	void onDraw(Renderer *renderer) override
    {
        Application::onDraw(renderer);
        
        drawScene(renderer);
        drawScreenTexture(renderer);
    }
    
    void drawScene(Renderer *renderer)
    {
        renderer->setCamera(&camera_);
        
        MaterialPtr material = getCurrentMaterial();
        material->bindShader();
        
        Vector3 lightDir = -lightCamera_.getForwardVector();
        lightDir = camera_.getViewMatrix().transformNormal(lightDir);
        lightDir.normalize();
        material->bindUniform("lightDir", lightDir);
       
        objects_->draw(renderer);
    }
    
    void drawScreenTexture(Renderer *renderer)
    {
        glDisable(GL_DEPTH_TEST);
        renderer->setViewMatrix(Matrix::Identity);
        renderer->setProjMatrix(Matrix::Identity);
        
        Matrix world;
        world.setTranslate(-0.8, -0.8f, 0.0f);
        renderer->setWorldMatrix(world);
        
        quadMaterial_->bindShader();
        quadMesh_->draw(renderer);
        
        glEnable(GL_DEPTH_TEST);
	}

	void onSizeChange(int width, int height) override
	{
		Application::onSizeChange(width, height);
		setupProjectionMatrix();
	}

	void setupProjectionMatrix()
	{
		camera_.setPerspective(PI_QUARTER, getAspect(), 1.0f, 30.0f);
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
    
    virtual void onKey(int key, int scancode, int action, int mods) override
    {
        Application::onKey(key, scancode, action, mods);
        
        if(action == GLFW_RELEASE)
        {
            switch(key)
            {
                case GLFW_KEY_M:
                    materialIndex_ = (materialIndex_ + 1) % 2;
                    break;
                    
                case GLFW_KEY_SPACE:
                    doRayTracing();
                    break;
            }
        }
    }
    
    MaterialPtr getCurrentMaterial()
    {
        return materials_[materialIndex_];
    }
    
    void doRayTracing()
    {
        for(int r = 0; r < winHeight_; ++r)
        {
            for(int c = 0; c < winWidth_; ++c)
            {
                int index = (r * winWidth_ + c) * 4;

                float px = float(c) / float(winWidth_) * 2.0f - 1.0f;
                float py = 1.0f - float(r) / float(winHeight_) * 2.0f;
                Ray ray = camera_.projectionPosToWorldRay(px, py);
                Color cr(0x7f7f7fff);
                rayTrace(cr, objects_, ray, 1);

                pixels_[index + 0] = cr.r255();
                pixels_[index + 1] = cr.g255();
                pixels_[index + 2] = cr.b255();
                pixels_[index + 3] = cr.a255();
            }
        }
        
        texture_->bind();
        
        int oldAlignment;
        glGetIntegerv(GL_PACK_ALIGNMENT, &oldAlignment);
        glPixelStorei(GL_PACK_ALIGNMENT, 1);
        
        GLenum format = (GLenum)TextureFormat::RGBA;
        
        GL_ASSERT(glTexImage2D(GL_TEXTURE_2D, 0, format, winWidth_, winHeight_,
                               0, format, GL_UNSIGNED_BYTE, pixels_.data()));
        
        glPixelStorei(GL_PACK_ALIGNMENT, oldAlignment);
    }
    
    void rayTrace(Color &color, TransformPtr object, const Ray &ray, int depth)
    {
        for (int i = 0; i < object->getNumComponents(); ++i)
        {
            ComponentPtr com = object->getComponentByIndex(i);
            Mesh *pMesh = dynamic_cast<Mesh*>(com.get());
            if (pMesh != nullptr)
            {
                Matrix worldToLocal = object->getWorldToLocalMatrix();
                Ray localRay = ray;
                localRay.applyMatrix(worldToLocal);
                if (localRay.intersectAABB(pMesh->getBoundingBox()))
                {
                    MeshRayVisitor visitor(localRay);
                    pMesh->iterateFaces(visitor);
                    if (visitor.intersected_)
                    {
                        color = Color::Red;
                    }
                }
            }
        }

        for (int i = 0; i < object->getNumChildren(); ++i)
        {
            rayTrace(color, object->getChildByIndex(i), ray, depth);
        }
    }

private:
    
	Camera          camera_;
	Vector2         lastCursorPos_;

    int             materialIndex_ = 0;
    MaterialPtr     materials_[2];

    TransformPtr    objects_;
    
    MeshPtr         cubeMesh_;

    Camera          lightCamera_;

    MaterialPtr     quadMaterial_;
    MeshPtr         quadMesh_;
    
    TexturePtr      texture_;
    int             winWidth_;
    int             winHeight_;
    std::vector<char> pixels_;
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
