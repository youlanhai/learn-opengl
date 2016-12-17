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

		const char *TextureFile = "alpha.png";
		groundTexture_ = TextureMgr::instance()->get(TextureFile);
		if (!groundTexture_)
		{
			return false;
		}

		if (!createVertexLightShader())
		{
			return false;
		}
		if (!createPixelLightShader())
		{
			return false;
		}

		curShader_ = shaderVertex_;

		mesh_ = createSimpleGround(Vector2(1.0f, 1.0f), 0.2f, 0.05f, 0.5f);
		mesh_->addMaterial(curShader_);

		LOG_DEBUG("num vertices: %d", (int)mesh_->getVertexBuffer()->count());
		LOG_DEBUG("num indices: %d", (int)mesh_->getIndexBuffer()->count());
		return true;
	}

	bool createVertexLightShader()
	{
		const char *ShaderFile = "shader/light_vertex.shader";
		shaderVertex_ = ShaderProgramMgr::instance()->get(ShaderFile);
		if (!shaderVertex_)
		{
			return false;
		}

		setupShader(shaderVertex_);
		return true;
	}

	bool createPixelLightShader()
	{
		const char *ShaderFile = "shader/light_pixel.shader";
		shaderPixel_ = ShaderProgramMgr::instance()->get(ShaderFile);
		if (!shaderPixel_)
		{
			return false;
		}

		setupShader(shaderPixel_);
		return true;
	}

	void setupShader(ShaderProgramPtr shader)
	{
		shader->bind();
		ShaderUniform *un = shader->findUniform("u_texture0");
		un->bindValue(groundTexture_.get());

		Vector3 lightDir(1, 1, 1);
		lightDir.normalize();

		un = shader->findUniform("lightDir");
		un->bindValue(lightDir);

		un = shader->findUniform("lightColor");
		un->bindValue(Vector3(1.5f, 1.5f, 1.5f));

		un = shader->findUniform("ambientColor");
		un->bindValue(Vector3(0.2f, 0.2f, 0.2f));
	}

	void onKey(int key, int scancode, int action, int mods) override
	{
		if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE)
		{
			if (curShader_ == shaderVertex_)
			{
				curShader_ = shaderPixel_;
				LOG_INFO("switch to pixel light.");
			}
			else
			{
				curShader_ = shaderVertex_;
				LOG_INFO("switch to vertex light.");
			}

			Mesh::Materials materials;
			materials.push_back(curShader_);
			mesh_->setMaterials(materials);
		}
	}

	void setupWorldMatrix()
	{
		curShader_->bind();
		ShaderUniform *mvp = curShader_->findUniform("MVP");
		if (mvp != nullptr)
		{
			Matrix mat;
			mat.setIdentity();
			//mat.setRotateX(glfwGetTime() * 0.5f);
			mat.setRotateX(3.14f / 3);
			mvp->bindValue(mat);
		}
		curShader_->unbind();
	}

	void onDraw() override
	{
		Application::onDraw();
		setupWorldMatrix();

		mesh_->draw();
	}

	ShaderProgramPtr shaderVertex_;
	ShaderProgramPtr shaderPixel_;
	ShaderProgramPtr curShader_;
	TexturePtr	groundTexture_;
	MeshPtr     mesh_;
};

int main()
{
    MyApplication app;
    if(app.createWindow(800, 600, "007-simple-light"))
    {
        app.mainLoop();
    }
    return 0;
}
