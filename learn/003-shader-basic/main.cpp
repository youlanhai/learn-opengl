#include "Application.h"
#include "SmartPointer.h"
#include "ShaderProgram.h"
#include "PathTool.h"
#include "LogTool.h"
#include "FileSystem.h"

class MyApplication : public Application
{
public:
    
    virtual void onCreate()
    {
        Application::onCreate();
        
        std::string rootPath = getExePath();
        std::string resPath;
        while(!rootPath.empty() && !isDir(resPath = joinPath(rootPath, "res")))
        {
            rootPath = getFilePath(rootPath);
        }
        
        LOG_INFO("resource path: %s", resPath.c_str());
        
        FileSystem::instance()->addSearchPath(resPath);
        FileSystem::instance()->addSearchPath(joinPath(resPath, "003"));
        
        shader_ = new ShaderProgram();
        if(!shader_->loadFromFile("test.shader"))
        {
            LOG_ERROR("Failed load shader.");
        }
    }
    
    virtual void onDraw()
    {
        Application::onDraw();
    }
    
private:
    SmartPointer<ShaderProgram>  shader_;
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
