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
        const char *TextureFile = "alpha.png";
        
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
        
        typedef VertexXYZNUV VertexType;
        
        std::vector<VertexType> vertices;
        std::vector<uint16_t> indices;
        createSimpleGround(vertices, indices, Vector2(1, 1), 0.3, 0.1, 0.4);
        
        VertexBufferPtr vb = new VertexBufferEx<VertexType>(BufferUsage::Static, vertices.size(), vertices.data());
        ib_ = new IndexBufferEx<uint16_t>(BufferUsage::Static, indices.size(), indices.data());
        
        va_ = new VertexAttribute();
        if(!va_->init(shader_.get(), vb.get(), VertexDeclMgr::instance()->get(VertexType::getType()).get()))
        {
            LOG_ERROR("Failed create vertex attribute.");
            return false;
        }
        
        shader_->bind();
        
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
        
        ShaderUniform *mvp = shader_->findUniform("u_matWorldViewProj");
        if(mvp != nullptr)
        {
            Matrix mat;
            mat.setRotateX(glfwGetTime());
            mvp->bindValue(mat);
        }
        
        va_->bind();
        ib_->bind();
        glDrawElements(GL_TRIANGLES, ib_->count(), GLenum(ib_->getIndexType()), 0);
        ib_->unbind();
        va_->unbind();
        shader_->unbind();
    }
    
    ShaderProgramPtr shader_;
    TexturePtr texture_;
    VertexAttributePtr va_;
    IndexBufferPtr  ib_;
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
