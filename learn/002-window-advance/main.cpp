#include "Application.h"

class MyApplication : public Application
{
public:
    
    virtual void onCreate()
    {
        Application::onCreate();
        
        glClearColor(0.3f, 0.3f, 0.3f, 0);
    }
    
    virtual void onDraw()
    {
        Application::onDraw();
    }
};

int main()
{
    MyApplication app;
    if(app.createWindow(640, 480, "002-window-advance"))
    {
        app.mainLoop();
    }
    return 0;
}
