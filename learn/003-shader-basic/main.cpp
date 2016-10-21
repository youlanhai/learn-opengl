#include "Application.h"
#include "SmartPointer.h"
#include "ShaderProgram.h"
#include "PathTool.h"
#include "LogTool.h"
#include "FileSystem.h"
#include "DemoTool.h"

struct Vertex
{
    float x, y, z;
    float r, g, b, a;
};

class MyApplication : public Application
{
public:
    
    MyApplication()
    : vb_(0)
    {}
    
    virtual void onCreate() override
    {
        Application::onCreate();
        
        std::string resPath = findResPath();
        LOG_INFO("resource path: %s", resPath.c_str());
        
        FileSystem::instance()->addSearchPath(resPath);
        FileSystem::instance()->addSearchPath(joinPath(resPath, "003"));
        FileSystem::instance()->dumpSearchPath();
        
        shader_ = new ShaderProgram();
        if(!shader_->loadFromFile("test.shader"))
        {
            LOG_ERROR("Failed load shader.");
        }
        
        int matrixLocation = shader_->getUniformLocation("matWorldViewProj");
        int posLocation = shader_->getAttribLocation("a_position");
        int crLocation = shader_->getAttribLocation("a_color");
        
        glEnableVertexAttribArray(posLocation);
        glVertexAttribPointer(posLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
        
        glEnableVertexAttribArray(crLocation);
        glVertexAttribPointer(crLocation, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(float) * 3));
        
        
        Vertex vertices[4] = {
            {-0.6f, -0.4f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f}, // left bottom
            {-0.6f, 0.4f, 0.0f,  0.0f, 1.0f, 0.0f, 1.0f}, // left top
            {0.6f, 0.4f, 0.0f,   0.0f, 0.0f, 1.0f, 1.0f}, // right top
            {0.6f, -0.4f, 0.0f,  1.0f, 0.0f, 0.0f, 1.0f}, // right bottom
        };
        
        glGenBuffers(1, &vb_);
        glBindBuffer(GL_ARRAY_BUFFER, vb_);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    }
    
    virtual void onDestroy() override
    {
        if(glIsBuffer(vb_))
        {
            glDeleteBuffers(1, &vb_);
        }
    }
    
    virtual void onDraw() override
    {
        Application::onDraw();
        
        shader_->use();
        
        glBindBuffer(GL_ARRAY_BUFFER, vb_);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
    
private:
    SmartPointer<ShaderProgram>  shader_;
    GLuint vb_;
};

int main()
{
    LOG_DEBUG("ExePath: %s", getExePath().c_str());
    
    MyApplication app;
    if(app.createWindow(640, 480, "003-shader-basic"))
    {
        app.mainLoop();
    }
    
    return 0;
}
