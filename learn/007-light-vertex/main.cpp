#include "Application.h"
#include "ShaderProgram.h"
#include "ShaderProgramMgr.h"
#include "ShaderUniform.h"
#include "TextureMgr.h"
#include "Vertex.h"
#include "VertexBuffer.h"
#include "VertexAttribute.h"
#include "VertexDeclaration.h"
#include "FileSystem.h"
#include "DemoTool.h"
#include "PathTool.h"
#include "LogTool.h"
#include "Matrix.h"
#include "Mesh.h"

class MyApplication : public Application
{
	bool onCreate() override
	{
		std::string resPath = findResPath();
		FileSystem::instance()->addSearchPath(resPath);
		FileSystem::instance()->addSearchPath(joinPath(resPath, "common"));
		FileSystem::instance()->dumpSearchPath();

		Texture::s_defaultQuality = TextureQuality::Nearest;

		const char *ShaderFile = "shader/light_pixel.shader";
		const char *TextureFile = "alpha.png";

		shader_ = ShaderProgramMgr::instance()->get(ShaderFile);
		TexturePtr texture = TextureMgr::instance()->get(TextureFile);
		if (!shader_ || !texture)
		{
			return false;
		}

		shader_->bind();
		ShaderUniform *un = shader_->findUniform("u_texture0");
		un->bindValue(texture.get());

		Vector3 lightDir(1, 1, 1);
		lightDir.normalize();

		un = shader_->findUniform("lightDir");
		un->bindValue(lightDir);
		un = shader_->findUniform("lightColor");
		un->bindValue(Vector3(1.0f, 1.0f, 1.0f));
		un = shader_->findUniform("ambientColor");
		un->bindValue(Vector3(0.2f, 0.2f, 0.2f));


		mesh_ = createSimpleGround(Vector2(1.0f, 1.0f), 0.2f, 0.01f, 0.5f);
		mesh_->addMaterial(shader_);

		LOG_DEBUG("num vertices: %d", (int)mesh_->getVertexBuffer()->count());
		LOG_DEBUG("num indices: %d", (int)mesh_->getIndexBuffer()->count());
		return true;
	}

	void onDraw() override
	{
		Application::onDraw();

		shader_->bind();

		ShaderUniform *mvp = shader_->findUniform("MVP");
		if (mvp != nullptr)
		{
			Matrix mat;
			mat.setIdentity();
			mat.setRotateX(glfwGetTime() * 0.5f);
			mvp->bindValue(mat);
		}

		mesh_->draw();
		shader_->unbind();
	}

	ShaderProgramPtr shader_;
	MeshPtr     mesh_;
};

int main()
{
    MyApplication app;
    if(app.createWindow(640, 480, "007-light-vertex"))
    {
        app.mainLoop();
    }
    return 0;
}
