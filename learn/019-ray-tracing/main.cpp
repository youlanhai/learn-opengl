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
#include "DebugDraw.h"

#include <algorithm>
#include <float.h>

class MeshInfo : public ReferenceCount
{
public:
    TransformPtr    transform_;
    MeshPtr         mesh_;

    std::vector<Vector3>    vertices_;
    std::vector<int>        indices_;
    AABB            boundingBox_;

public:
    MeshInfo(TransformPtr t, MeshPtr m, const Matrix &localToWorld)
    {
        transform_ = t;
        mesh_ = m;

        VertexBufferPtr vb = m->getVertexBuffer();
        IndexBufferPtr ib = m->getIndexBuffer();

        if(true)
        {
            vertices_.resize(vb->count());
            const char *data = vb->lock(true);
            for (size_t i = 0; i < vb->count(); ++i)
            {
                vertices_[i] = localToWorld.transformPoint(*(Vector3*)data);
                data += vb->stride();
            }
            vb->unlock();
        }

        if (ib)
        {
            indices_.resize(ib->count());
            const char *data = ib->lock(true);
            for (size_t i = 0; i < ib->count(); ++i)
            {
                indices_[i] = Mesh::extractIndex(data, ib->stride(), i);
            }
            ib->unlock();
        }
        else
        {
            indices_.resize(vertices_.size());
            for (int i = 0; i < (int)vertices_.size(); ++i)
            {
                indices_[i] = i;
            }
        }

        boundingBox_ = caculateBoundingBox();
    }

    AABB caculateBoundingBox()
    {
        AABB aabb;
        if (indices_.empty())
        {
            aabb.max_ = aabb.min_ = transform_->getPosition();
        }
        else
        {
            aabb.setEmpty();
            for (int index : indices_)
            {
                aabb.addPoint(vertices_[index]);
            }
        }
        return aabb;
    }
};

typedef SmartPointer<MeshInfo> MeshInfoPtr;

class TraceInfo
{
public:
    Ray     ray_;
    int     pixelIndex_ = 0;
    int     meshIndex_ = -1;
    int     triangles_[3];
    float   t_ = FLT_MAX;
    float   u_ = 0.0f;
    float   v_ = 0.0f;
    float   distanceToLight_ = 0.0f;
    bool    bShadow_ = false;

    TraceInfo()
    {}

    bool isValid() const { return meshIndex_ >= 0; }
};

class TraceManager
{
public:

    /// 场景中所有的模型
    std::vector<MeshInfoPtr> meshs_;
    /// 待执行的光线追踪程序
    std::list<TraceInfo>    pendingRays_;
    std::vector<char>       pixels_;

    size_t      totalRays_ = 0;
    Vector3     lightPosition_;
    float       lightRange_ = 20.0f;

public:
    TraceManager()
    {

    }

    bool isFinished() const { return pendingRays_.empty(); }

    void initTrace(int winWidth, int winHeight, Camera *camera)
    {
        pixels_.resize(winWidth * winHeight * 4);
        memset(pixels_.data(), 0, pixels_.size());

        for (int r = 0; r < winHeight; ++r)
        {
            for (int c = 0; c < winWidth; ++c)
            {
                int index = (r * winWidth + c) * 4;

                float px = float(c) / float(winWidth) * 2.0f - 1.0f;
                float py = 1.0f - float(r) / float(winHeight) * 2.0f;

                TraceInfo info;
                info.pixelIndex_ = index;
                info.ray_ = camera->projectionPosToWorldRay(px, py);
                pendingRays_.push_back(info);
            }
        }

        totalRays_ = pendingRays_.size();
        LOG_DEBUG("Total rays: %d", (int)pendingRays_.size());
    }

    void tick()
    {
        if (isFinished())
        {
            return;
        }

        TraceInfo info = pendingRays_.front();
        pendingRays_.pop_front();

        if (pendingRays_.size() % (totalRays_ / 100) == 0)
        {
            int progress = pendingRays_.size() * 100 / totalRays_;
            LOG_DEBUG("Ray trace progress: %d", 100 - progress);
        }

        rayTrace(info, 1);

        Color cr(0.1f, 0.1f, 0.1f);
        if (info.isValid())
        {
            float gray = 0.2f;
            if (!info.bShadow_)
            {
                float f = 1.0f / (0.1f + info.distanceToLight_);
                gray += f * 1.2f;
            }
            cr.set(gray, gray, gray);
        }

        int index = info.pixelIndex_;
        pixels_[index + 0] = (char)cr.r255();
        pixels_[index + 1] = (char)cr.g255();
        pixels_[index + 2] = (char)cr.b255();
        pixels_[index + 3] = (char)cr.a255();

        if (isFinished())
        {
            LOG_DEBUG("Ray trace finished.");
        }
    }

    void rayTrace(TraceInfo &info, int depth)
    {
        rayIntersect(info);

        if (!info.isValid())
        {
            return;
        }

        // 检测是否对光源可见
        Vector3 cross = info.ray_.origin_ + info.ray_.direction_ * (info.t_ - 0.1f);
        Vector3 normal = lightPosition_ - cross;
        float distanceToLight = normal.length();
        normal /= distanceToLight; // normalize

        info.distanceToLight_ = distanceToLight;

        TraceInfo infoToLight;
        infoToLight.ray_.origin_ = cross;
        infoToLight.ray_.direction_ = normal;

        rayIntersect(infoToLight);

        // 交点到灯光之间，有障碍物
        info.bShadow_ = infoToLight.isValid() && infoToLight.t_ < distanceToLight;
    }

    void rayIntersect(TraceInfo &info)
    {
        Ray &ray = info.ray_;
        for (size_t meshIndex = 0; meshIndex < meshs_.size(); ++meshIndex)
        {
            MeshInfoPtr mesh = meshs_[meshIndex];
            if (!ray.intersectAABB(mesh->boundingBox_))
            {
                continue;
            }

            for (size_t i = 0; i < mesh->indices_.size(); i += 3)
            {
                int i0 = mesh->indices_[i + 0];
                int i1 = mesh->indices_[i + 1];
                int i2 = mesh->indices_[i + 2];
                float t, u, v;
                if (ray.intersectTriangle(
                    mesh->vertices_[i0],
                    mesh->vertices_[i1],
                    mesh->vertices_[i2],
                    &t, &u, &v) && t < info.t_)
                {
                    info.meshIndex_ = meshIndex;
                    info.t_ = t;
                    info.u_ = u;
                    info.v_ = v;
                    info.triangles_[0] = i0;
                    info.triangles_[1] = i1;
                    info.triangles_[2] = i2;
                }
            }
        }
    }

    /// 递归收集transform上的所有mesh组件
    void collectMesh(TransformPtr &transform)
    {
        for (int i = 0; i < transform->getNumComponents(); ++i)
        {
            ComponentPtr com = transform->getComponentByIndex(i);
            Mesh *pMesh = dynamic_cast<Mesh*>(com.get());
            if (nullptr == pMesh)
            {
                continue;
            }

            Matrix localToWorld = transform->getLocalToWorldMatrix();
            MeshInfoPtr info = new MeshInfo(transform, pMesh, localToWorld);
            meshs_.push_back(info);
        }

        for (int i = 0; i < transform->getNumChildren(); ++i)
        {
            collectMesh(transform->getChildByIndex(i));
        }
    }
};

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
        winWidth_ = (int)size.x;
        winHeight_ = (int)size.y;

        texture_ = new Texture();
        texture_->create(0, winWidth_, winHeight_, TextureFormat::RGBA, nullptr, GL_UNSIGNED_BYTE);

        if (!createMaterials())
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


        quadMesh_ = createQuad(Vector2(0.6f, 0.6f));
        quadMesh_->addMaterial(quadMaterial_);

        traceMgr_.lightPosition_.set(-2, 5, 0);
        traceMgr_.lightRange_ = 50.0f;
        traceMgr_.collectMesh(objects_);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        return true;
    }

    bool createMaterials()
    {
        MaterialPtr material;

        material = new Material();
        materials_[0] = material;
        if (!material->loadShader("shader/light_pixel.shader"))
        {
            return false;
        }

        material = new Material();
        materials_[1] = material;
        if (!material->loadShader("shader/light_vertex.shader"))
        {
            return false;
        }

        for (MaterialPtr &mtl : materials_)
        {
            mtl->bindShader();
            mtl->loadTexture("u_texture0", "white.png");
            mtl->bindUniform("lightColor", Color(1.0f, 1.0f, 1.0f, 1.0f));
        }

        quadMaterial_ = new Material();
        if (!quadMaterial_->loadShader("shader/xyzuv.shader"))
        {
            return false;
        }
        quadMaterial_->setTexture("u_texture0", texture_);
        return true;
    }

    void onTick(float elapse) override
    {
        camera_.handleCameraMove();
        if (!traceMgr_.isFinished())
        {
            int step = winWidth_;
            for (int i = 0; i < step; ++i)
            {
                traceMgr_.tick();
            }
            savePixelToTexture();
        }
    }

    void onDraw(Renderer *renderer) override
    {
        Application::onDraw(renderer);

        drawScene(renderer);
        drawPick(renderer);
        drawScreenTexture(renderer);
    }

    void drawScene(Renderer *renderer)
    {
        renderer->setCamera(&camera_);

        MaterialPtr material = getCurrentMaterial();
        material->bindShader();

        Vector3 lightDir = -lightCamera_.getForwardVector();
        lightDir.normalize();
        material->bindUniform("lightDir", lightDir);

        objects_->draw(renderer);
    }

    void drawPick(Renderer *renderer)
    {
        Vector2 cursorPos = getCursorPos();
        Ray ray = camera_.screenPosToWorldRay(cursorPos.x, cursorPos.y);

        TraceInfo info;
        info.ray_ = ray;
        traceMgr_.rayIntersect(info);

        if(info.isValid())
        {
            MeshInfoPtr mesh = traceMgr_.meshs_[info.meshIndex_];

            DebugDraw::instance()->drawFilledTriangle(
                mesh->vertices_[info.triangles_[0]],
                mesh->vertices_[info.triangles_[1]],
                mesh->vertices_[info.triangles_[2]],
                Color::Red,
                Matrix::Identity);
        }

        DebugDraw::instance()->draw(renderer);
    }

    void drawScreenTexture(Renderer *renderer)
    {
        glDisable(GL_DEPTH_TEST);
        renderer->setViewMatrix(Matrix::Identity);
        renderer->setProjMatrix(Matrix::Identity);

        Matrix world;
        world.setTranslate(-0.7, -0.7f, 0.0f);
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

        if (action == GLFW_RELEASE)
        {
            switch (key)
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
        LOG_DEBUG("start ray tracing...");
        traceMgr_.initTrace(winWidth_, winHeight_, &camera_);
    }

    void savePixelToTexture()
    {
        texture_->bind();
        
        int oldAlignment;
        glGetIntegerv(GL_PACK_ALIGNMENT, &oldAlignment);
        glPixelStorei(GL_PACK_ALIGNMENT, 1);
        
        GLenum format = (GLenum)TextureFormat::RGBA;
        
        GL_ASSERT(glTexImage2D(GL_TEXTURE_2D, 0, format, winWidth_, winHeight_,
                               0, format, GL_UNSIGNED_BYTE, traceMgr_.pixels_.data()));
        
        glPixelStorei(GL_PACK_ALIGNMENT, oldAlignment);
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
    TraceManager    traceMgr_;
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
