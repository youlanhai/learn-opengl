#include "Application.h"
#include "ShaderProgramMgr.h"
#include "ShaderUniform.h"
#include "TextureMgr.h"
#include "FileSystem.h"
#include "DemoTool.h"
#include "PathTool.h"
#include "LogTool.h"
#include "Matrix.h"
#include "Mesh.h"

class MyApplication : public Application
{
    bool onCreate() override
    {
        std::string resPath = findResPath();
        FileSystem::instance()->addSearchPath(resPath);
        FileSystem::instance()->addSearchPath(joinPath(resPath, "common"));
        FileSystem::instance()->dumpSearchPath();
        
        Texture::s_defaultQuality = TextureQuality::Nearest;
        
        const char *ShaderFile = "shader/xyzuv.shader";
        const char *TextureFile = "rock.png";
        
        shader_ = ShaderProgramMgr::instance()->get(ShaderFile);
        TexturePtr texture = TextureMgr::instance()->get(TextureFile);
        if(!shader_ || !texture)
        {
            return false;
        }
        
        shader_->bind();
        ShaderUniform *tex = shader_->findUniform("u_texture0");
        if(tex != nullptr)
        {
            tex->bindValue(texture.get());
        }
        
        mesh_ = createSimpleGround(Vector2(1.0f, 1.0f), 0.3f, 0.1f, 0.4f);
		MaterialPtr material = new Material();
		material->setShader(shader_);
        mesh_->addMaterial(material);
        return true;
    }
    
    void onDraw() override
    {
        Application::onDraw();
        
        shader_->bind();
        ShaderUniform *mvp = shader_->findUniform("u_matWorldViewProj");
        if(mvp != nullptr)
        {
            Matrix mat;
            mat.setRotateX((float)glfwGetTime());
            mvp->bindValue(mat);
        }
        
        mesh_->draw();
        shader_->unbind();
    }
    
    ShaderProgramPtr shader_;
    MeshPtr     mesh_;
};

int main()
{
    MyApplication app;
    if(app.createWindow(640, 480, "006-mesh"))
    {
        app.mainLoop();
    }
    return 0;
}
