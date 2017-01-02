#include "Application.h"
#include "ShaderProgram.h"
#include "ShaderUniform.h"
#include "Texture.h"
#include "Vertex.h"
#include "VertexBuffer.h"
#include "VertexAttribute.h"
#include "VertexDeclaration.h"
#include "FileSystem.h"
#include "DemoTool.h"
#include "PathTool.h"
#include "LogTool.h"
#include "Matrix.h"

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
        
        shader_ = new ShaderProgram();
        if(!shader_->loadFromFile(ShaderFile))
        {
            LOG_ERROR("Failed to load shader: %s", ShaderFile);
            return false;
        }
        
        texture_ = new Texture();
        if(!texture_->load(TextureFile))
        {
            LOG_ERROR("Failed to load texture %s", TextureFile);
            return false;
        }
        
        VertexXYZUV vertices[4] = {
            {{-0.5f, -0.5f, 0.0f}, {0.0f, 1.0f}}, //left bottom
            {{-0.5f,  0.5f, 0.0f}, {0.0f, 0.0f}}, //left top 
            {{0.5f,  -0.5f, 0.0f}, {1.0f, 1.0f}}, //right bottom
            {{0.5f,   0.5f, 0.0f}, {1.0f, 0.0f}}, //right top 
        };
        
        VertexBufferPtr vb = new VertexBufferEx<VertexXYZUV>(BufferUsage::Static, 4, vertices);
        
        va_ = new VertexAttribute();
        if(!va_->init(vb.get(), VertexDeclMgr::instance()->get(VertexXYZUV::getType()).get()))
        {
            LOG_ERROR("Failed create vertex attribute.");
            return false;
        }
        
        shader_->bind();
        ShaderUniform *mvp = shader_->findUniform("u_matWorldViewProj");
        if(mvp != nullptr)
        {
            mvp->bindValue(Matrix::Identity);
        }
        ShaderUniform *tex = shader_->findUniform("u_texture0");
        if(tex != nullptr)
        {
            tex->bindValue(texture_.get());
        }
        return true;
    }
    
    void onDraw() override
    {
        Application::onDraw();
        
        shader_->bind();
        va_->bind();
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        va_->unbind();
        shader_->unbind();
    }
    
    ShaderProgramPtr shader_;
    TexturePtr texture_;
    VertexAttributePtr va_;
};

int main()
{
    MyApplication app;
    if(app.createWindow(640, 480, "005-texture"))
    {
        app.mainLoop();
    }
    return 0;
}
