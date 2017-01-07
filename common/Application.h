#ifndef COMMON_APPLICATION_H
#define COMMON_APPLICATION_H

#include "glconfig.h"
#include "Vector2.h"
#include <string>

class Application
{
public:
    Application();
    ~Application();
    
    virtual bool createWindow(int width, int height, const std::string &title);
    
    /** 消息循环。创建完窗口后，需要调用此函数。*/
    virtual void mainLoop();

    /** 从其他环境切换过来的时候，需要先激活当前的gl环境。*/
    void makeCurrent();
    
    GLFWwindow* getWindow(){ return pWindow_; }

	float getDeltaTime() const { return deltaTime_; }

	Vector2 getWindowSize();
	Vector2 getCursorPos();

	int getMouseState(int key) { return glfwGetMouseButton(pWindow_, key); }
	bool isMousePress(int key) { return getMouseState(key) == GLFW_PRESS; }

	int getKeyState(int key) { return glfwGetKey(pWindow_, key); }
	bool isKeyPress(int key) { return getKeyState(key) == GLFW_PRESS; }
    
public:
    // 内部方法，不要手动调用
    
    /** 键盘事件回调。*/
    virtual void onKey(int key, int scancode, int action, int mods);
    
    virtual void onMouseButton(int button, int action, int mods);
    
    virtual void onMouseMove(double x, double y);
    
    virtual void onMouseScroll(double xoffset, double yoffset);
    
    /** OpenGL错误回调。*/
    virtual void onError(int error, const char *description);
    
    virtual void onSizeChange(int width, int height);

    virtual void onFrameBufferSizeChange(int width, int height);

	virtual void onChar(uint32_t ch);
    
protected:

	virtual void onTick();
    
    /** 重载此函数，实现自己的渲染方法。*/
    virtual void onDraw();
    
    virtual bool onCreate();
    
    virtual void onDestroy();
    
    GLFWwindow*  pWindow_;
	float		deltaTime_;
};

extern Application *gApp;

#endif //COMMON_APPLICATION_H
