#include "Application.h"
#include "Vertex.h"
#include "VertexBuffer.h"
#include "VertexAttribute.h"
#include "VertexDeclaration.h"
#include "ShaderProgram.h"
#include "ShaderUniform.h"
#include "LogTool.h"
#include "FileSystem.h"
#include "DemoTool.h"
#include "PathTool.h"
#include "Matrix.h"

class MyApplication : public Application
{
public:
    
    MyApplication()
    {
//        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    }
    
    virtual bool onCreate() override
    {
        std::string resPath = findResPath();
        FileSystem::instance()->addSearchPath(resPath);
        FileSystem::instance()->addSearchPath(joinPath(resPath, "common"));
        FileSystem::instance()->dumpSearchPath();
        
		// ÄæÊ±ÕëË³Ðò
		VertexXYZColor vertices[4] = {
			{ { -0.5f, 0.5f, 0.0f },{ 0.0f, 1.0f, 0.0f, 1.0f } }, // left top
			{ { -0.5f, -0.5f, 0.0f },{ 1.0f, 0.0f, 0.0f, 1.0f } }, // left bottom
			{ { 0.5f, 0.5f, 0.0f },{ 0.0f, 0.0f, 1.0f, 1.0f } }, // right top
			{ { 0.5f, -0.5f, 0.0f },{ 1.0f, 0.0f, 0.0f, 1.0f } }, // right bottom
		};
        
        vb_ = new VertexBufferEx<VertexXYZColor>(BufferUsage::Static, 4, vertices);
        VertexDeclarationPtr decl = VertexDeclMgr::instance()->get(VertexXYZColor::getType());
        if(!decl)
        {
            LOG_ERROR("Failed find Vertex Declaration");
            return false;
        }
        
        shader_ = new ShaderProgram();
        if(!shader_->loadFromFile("shader/xyzcolor.shader"))
        {
            LOG_ERROR("Failed to load shader");
            return false;
        }
        
        shader_->bind();
        ShaderUniform *uniform = shader_->findUniform("u_matWorldViewProj");
        if(uniform != nullptr)
        {
            uniform->bindValue(Matrix::Identity);
        }
        
        va_ = new VertexAttribute();
        if(!va_->init(vb_.get(), decl.get()))
        {
            LOG_ERROR("Failed to init vertex attribute");
            return false;
        }

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
        return true;
    }
    
    virtual void onDestroy() override
    {
        
    }
    
    virtual void onDraw(Renderer *renderer) override
    {
        Application::onDraw(renderer);
        
        shader_->bind();
        va_->bind();
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        va_->unbind();
        shader_->unbind();
    }
    
    ShaderProgramPtr    shader_;
    VertexBufferPtr     vb_;
    VertexAttributePtr  va_;
};

int main()
{
    MyApplication app;
    if(app.createWindow(640, 480, "004-VertexBuffer"))
    {
        app.mainLoop();
    }
    return 0;
}
