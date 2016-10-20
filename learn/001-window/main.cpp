#include <GLFW/glfw3.h>

int main()
{
    if(!glfwInit())
    {
        return -1;
    }
    
    GLFWwindow *window = glfwCreateWindow(640, 480, "GLFW", NULL, NULL);
    if(!window)
    {
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    glClearColor(0, 0, 1, 0);
    
    while(!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);
        
        glfwSwapBuffers(window);
        
        glfwPollEvents();
    }
    
    glfwTerminate();
	return 0;
}
