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

		//mesh_ = createSimpleGround(Vector2(1.0f, 1.0f), 0.2f, 0.1f, 0.5f);
		//mesh_ = createPlane(Vector2(1.0f, 1.0f), 0.2f);
		mesh_ = createCube(Vector3(1.0f, 1.0f, 1.0f));
		mesh_->addMaterial(curShader_);

		LOG_DEBUG("num vertices: %d", (int)mesh_->getVertexBuffer()->count());
		LOG_DEBUG("num indices: %d", (int)mesh_->getIndexBuffer()->count());

		glEnable(GL_DEPTH_TEST);
		//glDisable(GL_CULL_FACE);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
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

		Vector3 lightDir(0, 1, 0);
		lightDir.normalize();

		un = shader->findUniform("lightDir");
		un->bindValue(lightDir);

		un = shader->findUniform("lightColor");
		un->bindValue(Vector3(1.0f));

		un = shader->findUniform("ambientColor");
		un->bindValue(Vector3(0.2f));
	}

	void onKey(int key, int scancode, int action, int mods) override
	{
		Application::onKey(key, scancode, action, mods);

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

		Matrix matWorld;
		matWorld.setIdentity();
		//matWorld.setRotateX(3.14f / 3);
		matWorld.setRotateX(glfwGetTime() * 0.5f);

		ShaderUniform *un;

		un = curShader_->findUniform("matWorld");
		un->bindValue(matWorld);

		Matrix matView;
		matView.lookAt(Vector3(1, 1, -2), Vector3::Zero, Vector3::YAxis);

		Matrix matProj;
		matProj.setIdentity();
		matProj.perspectiveProjectionGL(90, 800.0f / 600.0f, 1, 1000.0f);
		//matProj.orthogonalProjectionGL(10, 10, 0.0f, 1000.0f);

		un = curShader_->findUniform("matMVP");
		un->bindValue(matWorld * matView * matProj);

		curShader_->unbind();
	}

	void onDraw() override
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
