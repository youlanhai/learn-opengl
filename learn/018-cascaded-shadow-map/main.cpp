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

const int MaxCascades = 4;


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
        
        if(!createFrameBuffer(1024, 1024))
        {
            return false;
        }
        
        if(!createMaterials())
        {
            return false;
        }
        
        MaterialPtr material = getCurrentMaterial();
        
        groundMesh_ = createPlane(Vector2(10, 10), 1);
        groundMesh_->addMaterial(material);
        
        cubeMesh_ = createCube(Vector3(1, 1, 1));
        cubeMesh_->addMaterial(material);

        ground_ = new Transform();
        ground_->addComponent(groundMesh_);

        casters_ = new Transform();
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
            t->addComponent(cubeMesh_);
            casters_->addChild(t);
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
    
    bool createFrameBuffer(int width, int height)
    {
        int frameBufferWidth = 1024;
        int frameBufferHeight = 1024;
        
        frameBuffer_ = new FrameBuffer();
        frameBuffer_->init(frameBufferWidth, frameBufferHeight);
        
        Texture2DArray *texture = new Texture2DArray();
        cascadeTexture_ = texture;
        texture->create(0, frameBufferWidth, frameBufferHeight, TextureFormat::Depth, MaxCascades);
        texture->setQuality(TextureQuality::Linear);
        texture->setUWrap(TextureWrap::Clamp);
        texture->setVWrap(TextureWrap::Clamp);
        return true;
    }
    
    bool createMaterials()
    {
        MaterialPtr material;
        
        material = new Material();
        materials_[0] = material;
        if (!material->loadShader("shader/cascade_shadowmap.shader"))
        {
            return false;
        }
        material->loadTexture("u_texture0", "white.png");
        
        material = new Material();
        materials_[1] = material;
        if (!material->loadShader("shader/cascade_show_cascade.shader"))
        {
            return false;
        }
        
        for(MaterialPtr &mtl : materials_)
        {
            mtl->bindShader();
            mtl->bindUniform("lightColor", Color(1.0f, 1.0f, 1.0f, 1.0f));
            mtl->setTexture("cascadeTexture", cascadeTexture_);
        }
        
        
        lightMaterial_ = new Material();
        if (!lightMaterial_->loadShader("shader/xyz.shader"))
        {
            return false;
        }
        
        quadMaterial_ = new Material();
        if(!quadMaterial_->loadShader("shader/cascade_texture.shader"))
        {
            return false;
        }
        quadMaterial_->setTexture("u_texture0", cascadeTexture_);
        return true;
    }
    
    /* 生成步骤
     *  1. 计算相z方向的分割点，划分出子视锥体。
     *  2. 计算子视锥体的8个顶点，并转换到灯光空间。
     *  3. 在灯光空间中，计算出每个视锥体的包围盒。
     *  4. 根据包围盒，计算出灯光投影体的范围。
     */
    void updateCascades()
    {
        float zDistance = camera_.getZFar() - camera_.getZNear();
        float zStep = zDistance / nCascades;
        float tanHalfFov = tan(camera_.getFov() * 0.5f);
        float aspect = camera_.getAspect();
        
        // 该矩阵用于把点从相机空间下，转换到灯光空间下。
        Matrix lightSpaceMatrix;
        lightSpaceMatrix.multiply(camera_.getModelMatrix(), lightCamera_.getViewMatrix());

        float lastZ = camera_.getZNear();
        for(int i = 0; i < nCascades; ++i)
        {
            float zNear = lastZ;
            float zFar = zNear + zStep;
            
            cascadeSplits_[i] = zFar;
            lastZ = zFar;
            
            float halfNearHeight = zNear * tanHalfFov;
            float halfNearWidth = halfNearHeight * aspect;
            
            float halfFarHeight = zFar * tanHalfFov;
            float halfFarWidth = halfFarHeight * aspect;
            
            // 求子视锥体的8个顶点
            Vector3 points[8] = {
                {-halfNearWidth, -halfNearHeight, zNear}, //left-bottom
                {-halfNearWidth, halfNearHeight, zNear}, //left-top
                {halfNearWidth, halfNearHeight, zNear}, //right-top
                {halfNearWidth, -halfNearHeight, zNear}, //right-bottom
                {-halfFarWidth, -halfFarHeight, zFar}, //left-bottom
                {-halfFarWidth, halfFarHeight, zFar}, //left-top
                {halfFarWidth, halfFarHeight, zFar}, //right-top
                {halfFarWidth, -halfFarHeight, zFar}, //right-bottom
            };
            
            // 转换到灯光空间
            for(int i = 0; i < 8; ++i)
            {
                points[i] = lightSpaceMatrix.transformPoint(points[i]);
            }
            
            // 计算包围盒
            Vector3 maxV = points[0];
            Vector3 minV = points[0];
            for(int i = 1; i < 8; ++i)
            {
                maxV.x = std::max(maxV.x, points[i].x);
                maxV.y = std::max(maxV.y, points[i].y);
                maxV.z = std::max(maxV.z, points[i].z);
                
                minV.x = std::min(minV.x, points[i].x);
                minV.y = std::min(minV.y, points[i].y);
                minV.z = std::min(minV.z, points[i].z);
            }
            
            //TODO 根据场景物体，计算最小的near和far值
            float zn = minV.z, zf = maxV.z;
            
            // 构造灯光投影矩阵
            cascadeProjMatrices_[i].orthogonalProjectionOffCenterGL(minV.x, maxV.x, minV.y, maxV.y, zn, zf);
        }
      
        for(int i = nCascades; i < MaxCascades; ++i)
        {
            cascadeSplits_[i] = camera_.getZFar() + 2.0f;
        }
    }

	void onTick(float elapse) override
	{
		camera_.handleCameraMove();
	}

	void onDraw(Renderer *renderer) override
	{
        updateCascades();
        
        generateShadow(renderer);
        drawScene(renderer);
        drawCascadeTexture(renderer);
    }
    
    void generateShadow(Renderer *renderer)
    {
        glDisable(GL_CULL_FACE);
        renderer->setCamera(&lightCamera_);
        renderer->setOverwriteMaterial(lightMaterial_);
        frameBuffer_->bind();
        glDrawBuffer(0);
        glReadBuffer(0);
        
        Vector2 size = frameBuffer_->getSize();
        glViewport(0, 0, size.x, size.y);
        for(int i = 0; i < nCascades; ++i)
        {
            GL_ASSERT(glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, cascadeTexture_->getHandle(), 0, i));
            
            renderer->setProjMatrix(cascadeProjMatrices_[i]);
            
            glClearColor(0, 0, 0, 0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            //ground_->draw(renderer);
            casters_->draw(renderer);
        }
        
        frameBuffer_->unbind();
        renderer->setOverwriteMaterial(nullptr);
        glEnable(GL_CULL_FACE);
    }
    
    void drawScene(Renderer *renderer)
    {
        Vector2 size = getFrameBufferSize();
        glViewport(0, 0, size.x, size.y);
        glClearColor(0.15f, 0.24f, 0.24f, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderer->setCamera(&camera_);
        
        MaterialPtr material = getCurrentMaterial();
        groundMesh_->setMaterial(0, material);
        cubeMesh_->setMaterial(0, material);
        
        material->bindShader();
        
        Vector3 lightDir = -lightCamera_.getForwardVector();
        lightDir = camera_.getViewMatrix().transformNormal(lightDir);
        lightDir.normalize();
        material->bindUniform("lightDir", lightDir);
        material->bindUniform("lightViewMatrix", lightCamera_.getViewMatrix());
        
        ShaderUniform *un;
        un = material->findUniform("cascadeProjMatrices");
        if(un)
        {
            un->bindValue(cascadeProjMatrices_, MaxCascades, false);
        }
        
        un = material->findUniform("cascadeSplits");
        if(un)
        {
            un->bindValue(cascadeSplits_, MaxCascades);
        }

        ground_->draw(renderer);
        casters_->draw(renderer);
    }
    
    // 在屏幕上渲染cascade贴图
    void drawCascadeTexture(Renderer *renderer)
    {
        glDisable(GL_DEPTH_TEST);
        renderer->setViewMatrix(Matrix::Identity);
        renderer->setProjMatrix(Matrix::Identity);
        float startX = -0.8f;
        for(int i = 0; i < nCascades; ++i)
        {
            Matrix world;
            world.setTranslate(startX, -0.8f, 0.0f);
            renderer->setWorldMatrix(world);
            
            quadMaterial_->bindShader();
            quadMaterial_->bindUniform("layerIndex", i);
            quadMesh_->draw(renderer);
            
            startX += 0.45f;
        }
        glEnable(GL_DEPTH_TEST);
	}

	void onSizeChange(int width, int height) override
	{
		Application::onSizeChange(width, height);
		setupProjectionMatrix();
	}

	void setupProjectionMatrix()
	{
		camera_.setPerspective(PI_QUARTER, getAspect(), 1.0f, 20.0f);
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
                case GLFW_KEY_1:
                    nCascades = 1;
                    break;
                case GLFW_KEY_2:
                    nCascades = 2;
                    break;
                case GLFW_KEY_3:
                    nCascades = 3;
                    break;
                case GLFW_KEY_4:
                    nCascades = 4;
                    break;
                case GLFW_KEY_M:
                    materialIndex_ = (materialIndex_ + 1) % 2;
                    break;
            }
        }
    }
    
    MaterialPtr getCurrentMaterial()
    {
        return materials_[materialIndex_];
    }

private:
    
	Camera          camera_;
	Vector2         lastCursorPos_;

    int             materialIndex_ = 0;
    MaterialPtr     materials_[2];

    TransformPtr    casters_;
    TransformPtr    ground_;
    
    MeshPtr         groundMesh_;
    MeshPtr         cubeMesh_;

    Camera          lightCamera_;
    MaterialPtr     lightMaterial_;
    
    FrameBufferPtr  frameBuffer_;
    
    int             nCascades = 4;
    Matrix          cascadeProjMatrices_[MaxCascades];
    float           cascadeSplits_[MaxCascades];
    TexturePtr      cascadeTexture_;
    
    MaterialPtr     quadMaterial_;
    MeshPtr         quadMesh_;
    
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
