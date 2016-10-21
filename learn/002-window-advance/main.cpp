#include "Application.h"

int main()
{
    Application app;
    if(app.createWindow(640, 480, "002-window-advance"))
    {
        app.mainLoop();
    }
    return 0;
}
