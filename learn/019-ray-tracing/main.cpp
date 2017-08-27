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
    /// 测试材质
    struct Material
    {
        // 表面颜色
        Vector4 color;
        // 反射系数
        float   reflection;
        // 折射系数
        float   refraction;
    };

    TransformPtr    transform_;
    MeshPtr         mesh_;
    Material        material_;

    struct Face
    {
        int i[3];
        Vector3 normal;
    };

    std::vector<Vector3>    vertices_;
    std::vector<Face>       faces_;
    AABB            boundingBox_;

public:
    MeshInfo(TransformPtr t, MeshPtr m, const Material &mtl, const Matrix &localToWorld)
    {
        transform_ = t;
        mesh_ = m;
        material_ = mtl;

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
            faces_.resize(ib->count() / 3);
            const char *data = ib->lock(true);
            for (size_t i = 0; i < faces_.size(); ++i)
            {
                Face &f = faces_[i];
                f.i[0] = Mesh::extractIndex(data, ib->stride(), i * 3 + 0);
                f.i[1] = Mesh::extractIndex(data, ib->stride(), i * 3 + 1);
                f.i[2] = Mesh::extractIndex(data, ib->stride(), i * 3 + 2);
            }
            ib->unlock();
        }
        else
        {
            faces_.resize(vertices_.size() / 3);
            for (size_t i = 0; i < faces_.size(); ++i)
            {
                Face &f = faces_[i];
                f.i[0] = i * 3 + 0;
                f.i[1] = i * 3 + 1;
                f.i[2] = i * 3 + 2;
            }
        }

        boundingBox_ = caculateBoundingBox();

        for (Face &face : faces_)
        {
            calculateNormal(face);
        }
    }

    AABB caculateBoundingBox()
    {
        AABB aabb;
        if (faces_.empty())
        {
            aabb.max_ = aabb.min_ = transform_->getPosition();
        }
        else
        {
            aabb.setEmpty();
            for (Face &f : faces_)
            {
                aabb.addPoint(vertices_[f.i[0]]);
                aabb.addPoint(vertices_[f.i[1]]);
                aabb.addPoint(vertices_[f.i[2]]);
            }
        }
        return aabb;
    }

    void calculateNormal(Face &face)
    {
        Vector3 &a = vertices_[face.i[0]];
        Vector3 &b = vertices_[face.i[1]];
        Vector3 &c = vertices_[face.i[2]];

        face.normal.crossProduct(c - a, b - a);
        face.normal.normalize();
    }
};

typedef SmartPointer<MeshInfo> MeshInfoPtr;

class TraceInfo
{
public:
    Ray     ray_;
    int     pixelIndex_ = 0;
    int     meshIndex_ = -1;
    int     face_;
    float   t_ = FLT_MAX;
    float   u_ = 0.0f;
    float   v_ = 0.0f;
    Vector4 color_;
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
    Vector4     lightColor_;
    Vector3     lightPosition_;
    Vector3     viewPosition_;
    Vector4     ambientColor_;
    /// 每次反射后的衰减
    float       bounceAttenuation_ = 1.0f;

public:
    TraceManager()
        : lightColor_(1.0f, 1.0f, 1.0f, 1.0f)
        , ambientColor_(0.2f, 0.2f, 0.2f, 1.0f)
    {

    }

    bool isFinished() const { return pendingRays_.empty(); }

    void initTrace(int winWidth, int winHeight, Camera *camera)
    {
        viewPosition_ = camera->getPosition();

        pixels_.resize(winWidth * winHeight * 4);
        memset(pixels_.data(), 0, pixels_.size());

        pendingRays_.clear();

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

        Vector4 cr = ambientColor_;
        if (info.isValid())
        {
            cr = info.color_;
            cr += ambientColor_ * meshs_[info.meshIndex_]->material_.color;
        }

        int index = info.pixelIndex_;
        pixels_[index + 0] = (char)clamp(int(cr.x * 255), 0, 255);
        pixels_[index + 1] = (char)clamp(int(cr.y * 255), 0, 255);
        pixels_[index + 2] = (char)clamp(int(cr.z * 255), 0, 255);
        pixels_[index + 3] = (char)clamp(int(cr.w * 255), 0, 255);

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

        const Ray &ray = info.ray_;

        // 添加一点偏移，防止拾取到自己
        const float epsilon = 0.01f;
        Vector3 position = ray.origin_ + ray.direction_ * info.t_;
       
        MeshInfoPtr &mesh = meshs_[info.meshIndex_];
        MeshInfo::Face &face = mesh->faces_[info.face_];

        // 检测是否对光源可见
        do
        {
            Vector3 lightDir = lightPosition_ - position;
            float distanceToLight = lightDir.length();
            lightDir /= distanceToLight; // normalize

            TraceInfo lightTrace;
            lightTrace.ray_.origin_ = position + lightDir * epsilon;
            lightTrace.ray_.direction_ = lightDir;
            distanceToLight -= epsilon;

            rayIntersect(lightTrace);

            // 交点到灯光之间，有障碍物
            info.bShadow_ = lightTrace.isValid() && lightTrace.t_ < distanceToLight;
            if (!info.bShadow_)
            {
                info.color_ += lightVertex(position, face.normal, lightPosition_, lightColor_,
                    viewPosition_, mesh->material_, 1.0f, 0.2f, 0.0f);
            }
        } while (0);

        // 反射光线
        if(depth-- > 0 && mesh->material_.reflection > 0.0f)
        {
            Vector3 reflactDir = reflect(-ray.direction_, face.normal); 

            TraceInfo reflectTrace;
            reflectTrace.ray_.origin_ = position + reflactDir * epsilon; // 添加一点偏移，防止拾取到自己
            reflectTrace.ray_.direction_ = reflactDir;

            rayTrace(reflectTrace, depth);

            if (reflectTrace.isValid())
            {
                // 以相交点为灯光，照射当前点
                MeshInfo::Material &lightMtl = meshs_[reflectTrace.meshIndex_]->material_;
                Vector3 lightPos = reflectTrace.ray_.origin_ + reflactDir * reflectTrace.t_;
                // 加上环境光颜色
                Vector4 lightColor = reflectTrace.color_ + lightMtl.color * ambientColor_;

                Vector4 color = lightVertex(position, face.normal, lightPos, lightColor,
                    viewPosition_, mesh->material_, 1.0f, 1.0f, 0.1f);
                info.color_ += color * (lightMtl.reflection * bounceAttenuation_);
            }
        }
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

            for (size_t i = 0; i < mesh->faces_.size(); ++i)
            {
                MeshInfo::Face &f = mesh->faces_[i];
                float t, u, v;
                if (ray.intersectTriangle(
                    mesh->vertices_[f.i[0]],
                    mesh->vertices_[f.i[1]],
                    mesh->vertices_[f.i[2]],
                    &t, &u, &v) && t < info.t_)
                {
                    info.meshIndex_ = meshIndex;
                    info.t_ = t;
                    info.u_ = u;
                    info.v_ = v;
                    info.face_ = (int)i;
                }
            }
        }
    }

    /** 计算反射向量
    *   @param  ray     输入向量的负方向向量
    *   @param  normal  平面的法向量
    */
    static Vector3 reflect(const Vector3 &ray, const Vector3 &normal)
    {
        Vector3 ret = normal * (normal.dotProduct(ray) * 2.0f) - ray;
        ret.normalize();
        return ret;
    }

    /** 顶点光照计算
    *   @param  position    顶点位置
    *   @param  normal      顶点的法线
    *   @param  lightPos    灯光位置
    */
    static Vector4 lightVertex(
        const Vector3 &position,
        const Vector3 &normal,
        const Vector3 &lightPos,
        const Vector4 &lightColor,
        const Vector3 &viewPos,
        const MeshInfo::Material &material,
        float attK0, float attk1, float attK2)
    {
        Vector3 lightDir = lightPos - position;
        float distanceToLight = lightDir.length();
        lightDir /= distanceToLight;

        Vector4 ret;
        // 漫反射
        float diffuse = normal.dotProduct(lightDir);
        if (diffuse <= 0.0f)
        {
            return ret;
        }

        float indensity = diffuse;

        // 镜面反射
        Vector3 reflectDir = reflect(lightDir, normal);
        Vector3 viewDir = viewPos - position;
        viewDir.normalize();

        float specular = reflectDir.dotProduct(viewDir);
        if (specular > 0)
        {
            indensity += std::pow(specular, 32);
        }

        float attenuation = 1.0f / (1.0f + distanceToLight * 0.1f + distanceToLight * distanceToLight * attK2);
        ret = material.color * lightColor * (indensity * attenuation);
        return ret;
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

        MeshInfo::Material materials[] = {
            { { 1.0f, 1.0f, 1.0f, 1.0f }, 1.0f, 0.0f }, // 0 白色 反射
            { { 1.0f, 1.0f, 1.0f, 1.0f }, 0.1f, 0.0f }, // 1 白色
            { { 0.5f, 0.0f, 0.0f, 1.0f }, 0.1f, 0.0f }, // 2 红色
            { { 1.0f, 0.0f, 0.0f, 1.0f }, 0.3f, 0.0f }, // 3 红色 反射
            { { 0.0f, 0.0f, 1.0f, 1.0f }, 0.3f, 1.0f }, // 4 蓝色 折射
            { { 0.0f, 1.0f, 0.0f, 1.0f }, 0.3f, 0.0f }, // 5 绿色
        };

        float objectDatas[][5] = {
            {0,  -5, 0, 10, 0}, // bottom
            {0,  15, 0, 10, 1}, // top
            {-10, 5, 0, 10, 1}, // left
            {10,  5, 0, 10, 2}, // right
            {0,  5, 10, 10, 0}, // back
            {0, 0.50f, 0, 1.0f, 1}, // 中心
            {0, 1.25f, 0, 0.5f, 3}, // 上侧
            {-1.5f, 0.5f, 0, 1, 4}, // 左侧
            { 2.5f, 0.5f, 0, 1.2, 5}, // 右侧
        };

        for (float *p : objectDatas)
        {
            TransformPtr t = new Transform();
            t->setPosition(Vector3(p[0], p[1], p[2]));
            t->setScale(p[3]);
            t->addComponent(cubeMesh_);
            objects_->addChild(t);

            int mtlIndex = (int)p[4];
            MeshInfoPtr m = new MeshInfo(t, cubeMesh_, materials[mtlIndex], t->getLocalToWorldMatrix());
            traceMgr_.meshs_.push_back(m);
        }

        camera_.lookAt(Vector3(0, 3, -10), Vector3::Zero, Vector3::YAxis);
        setupProjectionMatrix();
        Renderer::instance()->setCamera(&camera_);

        lightCamera_.setPosition(-2, 5, -4);
        lightCamera_.setRotation(PI_FULL / 6.0f, PI_FULL / 3.0f, 0.0f);
        lightCamera_.setOrtho(20, 20, 1, 10);


        quadMesh_ = createQuad(Vector2(2.0f, 2.0f));
        quadMesh_->addMaterial(quadMaterial_);

        traceMgr_.lightPosition_ = lightCamera_.getPosition();

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

        if (bShowRayTrace_)
        {
            drawScreenTexture(renderer);
        }
        else
        {
            drawScene(renderer);
            drawPick(renderer);
        }
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

            MeshInfo::Face &f = mesh->faces_[info.face_];

            DebugDraw::instance()->drawFilledTriangle(
                mesh->vertices_[f.i[0]],
                mesh->vertices_[f.i[1]],
                mesh->vertices_[f.i[2]],
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
        world.setIdentity();
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

            case GLFW_KEY_R:
                bShowRayTrace_ = !bShowRayTrace_;
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

        bShowRayTrace_ = true;
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
    bool            bShowRayTrace_ = false;
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
