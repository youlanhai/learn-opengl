#include "Application.h"
#include <cstdlib>
#include <cassert>


Application *gApp = nullptr;

static void errorCallback(int error, const char *description)
{
    if(gApp != nullptr)
    {
        gApp->onError(error, description);
    }
}

static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if(window == gApp->getWindow())
    {
        gApp->onKey(key, scancode, action, mods);
    }
}


Application::Application()
: pWindow_(nullptr)
{
    gApp = this;
    
    glfwSetErrorCallback(errorCallback);
	if(!glfwInit())
	{
		exit(0);
	}
}

Application::~Application()
{
    if(pWindow_ != nullptr)
    {
        glfwDestroyWindow(pWindow_);
    }
	glfwTerminate();
    if(gApp == this)
    {
        gApp = nullptr;
    }
}

bool Application::createWindow(int width, int height, const std::string &title)
{
    assert(pWindow_ == nullptr);
    
    pWindow_ = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if(nullptr == pWindow_)
    {
        return false;
    }
    
    makeCurrent();
    
    glfwSetKeyCallback(pWindow_, keyCallback);
    
    onCreate();
    return true;
}

void Application::makeCurrent()
{
    glfwMakeContextCurrent(pWindow_);
}

void Application::mainLoop()
{
    while(!glfwWindowShouldClose(pWindow_))
    {
        draw();
        glfwSwapBuffers(pWindow_);
        glfwPollEvents();
    }
}

void Application::draw()
{
    glClear(GL_COLOR_BUFFER_BIT);
}

void Application::onCreate()
{
    // 开启垂直同步
    glfwSwapInterval(1);
    // 屏幕清成蓝色
    glClearColor(0, 0, 1, 0); 
}

void Application::onKey(int key, int scancode, int action, int modes)
{
    if(key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE)
        glfwSetWindowShouldClose(pWindow_, GLFW_TRUE);
}

void Application::onError(int error, const char *description)
{
    fprintf(stderr, "Error(%d): %s\n", error, description);
}
