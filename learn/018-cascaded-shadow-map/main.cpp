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

		camera_.lookAt(Vector3(0, 3, -10), Vector3::Zero, Vector3::YAxis);
		setupViewProjMatrix();
		Renderer::instance()->setCamera(&camera_);

        lightCamera_.setPosition(0, 6, -6);
        lightCamera_.setRotation(PI_FULL / 6.0f, PI_FULL / 3.0f, 0.0f);
        lightCamera_.setOrtho(20, 20, 1, 10);

        int frameBufferWidth = 1024;
        int frameBufferHeight = 1024;
        
        frameBuffer_ = new FrameBuffer();
        frameBuffer_->init(frameBufferWidth, frameBufferHeight);
        
        
        Texture2DArray *texture = new Texture2DArray();
        cascadeTexture_ = texture;
        texture->create(0, frameBufferWidth, frameBufferHeight, TextureFormat::Depth, MaxCascades);
        texture->setQuality(TextureQuality::Nearest);
        texture->setUWrap(TextureWrap::Clamp);
        texture->setVWrap(TextureWrap::Clamp);
        
        material_->setTexture("cascadeTexture", texture);
        
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
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
        float halfFov = camera_.getFov() * 0.5f;
        float aspect = camera_.getAspect();
        
        Matrix lightMatrix = lightCamera_.getViewMatrix();

        Vector3 forward = camera_.getForwardVector();
        Vector3 up = camera_.getUpVector();
        Vector3 right = camera_.getRightVector();
        
        float lastZ = lightCamera_.getZNear();
        for(int i = 0; i < nCascades; ++i)
        {
            float zNear = lastZ;
            float zFar = zNear + zStep;
            
            cascadeSplits_[i] = zFar;
            lastZ = zFar;
            
            // 求子视锥体的8个顶点
            Vector3 points[8];
            
            float halfHeight = zNear * tan(halfFov);
            float halfWidth = halfHeight * aspect;
            Vector3 vNear = forward * zNear;
            Vector3 dw = right * halfWidth;
            Vector3 dh = up * halfHeight;
            points[0] = vNear - dw - dh; // left bottom
            points[1] = vNear - dw + dh; // left top
            points[2] = vNear + dw + dh; // right top
            points[3] = vNear + dw - dh; // right bottom
            
            Vector3 vFar = forward * zFar;
            halfHeight = zFar * tan(halfFov);
            halfWidth = halfHeight * aspect;
            dw = right * halfWidth;
            dh = up * halfHeight;
            points[4] = vFar - dw - dh; // left bottom
            points[5] = vFar - dw + dh; // left top
            points[6] = vFar + dw + dh; // right top
            points[7] = vFar + dw - dh; // right bottom
            
            // 转换到灯光空间
            for(int i = 0; i < 8; ++i)
            {
                points[i] = lightMatrix.transformPoint(points[i]);
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
            
            // 构造灯光投影矩阵
            cascadeProjMatrices_[i].orthogonalProjectionOffCenterGL(minV.x, maxV.x, minV.y, maxV.y, lightCamera_.getZNear(), lightCamera_.getZFar());
            //cascadeProjMatrices_[i] = lightCamera_.getProjMatrix();
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
        
        if(true)
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

        Vector2 size = getFrameBufferSize();
        glViewport(0, 0, size.x, size.y);
        glClearColor(0.15f, 0.24f, 0.24f, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderer->setCamera(&camera_);
        material_->bindShader();
        material_->bindUniform("lightDir", -lightCamera_.getForwardVector());
        material_->bindUniform("lightMatrix", lightCamera_.getViewMatrix());
        ShaderUniform *un = material_->findUniform("cascadeProjMatrices");
        if(un)
        {
            un->bindValue(cascadeProjMatrices_, MaxCascades, false);
        }
        un = material_->findUniform("cascadeSplits");
        if(un)
        {
            un->bindValue(cascadeSplits_, MaxCascades);
        }

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
		camera_.setPerspective(PI_QUARTER, size.x / size.y, 1.0f, 20.0f);
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

private:
    
	Camera          camera_;
	Vector2         lastCursorPos_;

    MaterialPtr     material_;

    TransformPtr    casters_;
    TransformPtr    ground_;

    Camera          lightCamera_;
    MaterialPtr     lightMaterial_;
    
    FrameBufferPtr  frameBuffer_;
    
    int             nCascades = 4;
    Matrix          cascadeProjMatrices_[MaxCascades];
    float           cascadeSplits_[MaxCascades];
    TexturePtr      cascadeTexture_;
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
