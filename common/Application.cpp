#include "Application.h"

#include <cstdlib>
#include <cassert>

#include "FileSystem.h"

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

static void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
    if(window == gApp->getWindow())
    {
        gApp->onMouseButton(button, action, mods);
    }
}

static void mouseMoveCallback(GLFWwindow *window, double x, double y)
{
    if(window == gApp->getWindow())
    {
        gApp->onMouseScroll(x, y);
    }
}

static void mouseScrollCallback(GLFWwindow *window, double xoffset, double yoffset)
{
    if(window == gApp->getWindow())
    {
        gApp->onMouseScroll(xoffset, yoffset);
    }
}

static void frameBufferSizeChangeCallback(GLFWwindow *window, int width, int height)
{
    if(window == gApp->getWindow())
    {
        gApp->onSizeChange(width, height);
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
    
    FileSystem::initInstance();
}

Application::~Application()
{
    if(pWindow_ != nullptr)
    {
        onDestroy();
        glfwDestroyWindow(pWindow_);
    }
    
    FileSystem::finiInstance();
    
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
    glfwSetMouseButtonCallback(pWindow_, mouseButtonCallback);
    glfwSetScrollCallback(pWindow_, mouseScrollCallback);
    glfwSetCursorPosCallback(pWindow_, mouseMoveCallback);
    glfwSetFramebufferSizeCallback(pWindow_, frameBufferSizeChangeCallback);
    
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
        onDraw();
        glfwSwapBuffers(pWindow_);
        glfwPollEvents();
    }
}

void Application::onDraw()
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

void Application::onDestroy()
{
    
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

void Application::onSizeChange(int width, int height)
{
    glViewport(0, 0, width, height);
}

void Application::onMouseButton(int button, int action, int mods)
{
    
}

void Application::onMouseMove(double x, double y)
{
    
}

void Application::onMouseScroll(double xoffset, double yoffset)
{
    
}
