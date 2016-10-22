#include "Application.h"

class MyApplication : public Application
{
public:
    
    virtual bool onCreate() override
    {
        Application::onCreate();
        
        glClearColor(0.3f, 0.3f, 0.3f, 0);
		return true;
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
