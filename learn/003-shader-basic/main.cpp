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
    {
	}
    
    virtual bool onCreate() override
    {
        Application::onCreate();
        
        std::string resPath = findResPath();
        LOG_INFO("resource path: %s", resPath.c_str());
        
        FileSystem::instance()->addSearchPath(resPath);
        FileSystem::instance()->addSearchPath(joinPath(resPath, "common"));
        FileSystem::instance()->dumpSearchPath();
        
        shader_ = new ShaderProgram();
        if(!shader_->loadFromFile("shader/xyzcolor.shader"))
        {
            LOG_ERROR("Failed load shader.");
			return false;
        }
        shader_->bind();

        int matrixLocation = shader_->getUniformLocation("u_matWorldViewProj");
        int posLocation = shader_->getAttribLocation("a_position");
        int crLocation = shader_->getAttribLocation("a_color");

		float matrix[16] = {
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1,
		};
		if (matrixLocation >= 0)
		{
            glUniformMatrix4fv(matrixLocation, 1, GL_FALSE, matrix);
		}
        
        // 逆时针排列三角形顶点
        Vertex vertices[4] = {
            {-0.6f, 0.4f, 0.0f,  0.0f, 1.0f, 0.0f, 1.0f}, // left top
            {-0.6f, -0.4f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f}, // left bottom
            {0.6f, 0.4f, 0.0f,   0.0f, 0.0f, 1.0f, 1.0f}, // right top
            {0.6f, -0.4f, 0.0f,  1.0f, 0.0f, 0.0f, 1.0f}, // right bottom
        };

        glGenVertexArrays(1, &vao_);
        glBindVertexArray(vao_);
        
        glGenBuffers(1, &vbo_);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		// 设置属性必须先绑定顶点buffer
		glEnableVertexAttribArray(posLocation);
		glVertexAttribPointer(posLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

		glEnableVertexAttribArray(crLocation);
		glVertexAttribPointer(crLocation, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(float) * 3));

        glBindVertexArray(0);

        glEnable(GL_CULL_FACE);
		return true;
    }
    
    virtual void onDestroy() override
    {
        if(glIsBuffer(vbo_))
        {
            glDeleteBuffers(1, &vbo_);
        }
        if (glIsVertexArray(vao_))
        {
            glDeleteVertexArrays(1, &vao_);
        }
    }
    
    virtual void onDraw(Renderer *renderer) override
    {
        Application::onDraw(renderer);

        shader_->bind();
        glBindVertexArray(vao_);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
    
private:
    SmartPointer<ShaderProgram>  shader_;
    GLuint vbo_ = 0;
    GLuint vao_ = 0;
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
