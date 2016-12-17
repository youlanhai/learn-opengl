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

const float PI = 3.141592f;

class MyApplication : public Application
{
	bool onCreate() override
	{
		std::string resPath = findResPath();
		FileSystem::instance()->addSearchPath(resPath);
		FileSystem::instance()->addSearchPath(joinPath(resPath, "common"));
		FileSystem::instance()->dumpSearchPath();

		const char *TextureFile = "alpha.png";
		groundTexture_ = TextureMgr::instance()->get(TextureFile);
		if (!groundTexture_)
		{
			return false;
		}
		groundTexture_->setQuality(TextureQuality::Nearest);

		if (!createVertexLightShader())
		{
			return false;
		}
		if (!createPixelLightShader())
		{
			return false;
		}

		curShader_ = shaderVertex_;

		mesh_ = createSimpleGround(Vector2(5.0f, 5.0f), 0.6f, 0.2f, 1.6f);
		//mesh_ = createPlane(Vector2(1.0f, 1.0f), 0.2f);
		//mesh_ = createCube(Vector3(1.0f, 1.0f, 1.0f));
		mesh_->addMaterial(curShader_);

		LOG_DEBUG("num vertices: %d", (int)mesh_->getVertexBuffer()->count());
		LOG_DEBUG("num indices: %d", (int)mesh_->getIndexBuffer()->count());

		Matrix matView;
		matView.lookAt(Vector3(0, 2, -1), Vector3::Zero, Vector3::YAxis);

		Matrix matProj;
		matProj.setIdentity();
		matProj.perspectiveProjectionGL(90, 800.0f / 600.0f, 1, 1000.0f);

		matViewProj = matView * matProj;

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

		Vector3 lightDir(1, 1, 0);
		lightDir.normalize();

		un = shader->findUniform("lightDir");
		un->bindValue(lightDir);

		un = shader->findUniform("lightColor");
		un->bindValue(Vector3(1.0f));

		un = shader->findUniform("ambientColor");
		un->bindValue(Vector3(0.1f));
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
		else if (key == GLFW_KEY_L && action == GLFW_RELEASE)
		{
			isLineMode = !isLineMode;
			if(isLineMode)
			{
				glPolygonMode(GL_FRONT, GL_LINE);
			}
			else
			{
				glPolygonMode(GL_FRONT, GL_FILL);
			}
		}
	}

	void setupWorldMatrix()
	{
		curShader_->bind();

		Matrix matWorld;
		matWorld.setRotateY(glfwGetTime() * 0.5f);

		ShaderUniform *un;

		un = curShader_->findUniform("matWorld");
		un->bindValue(matWorld);

		un = curShader_->findUniform("matMVP");
		un->bindValue(matWorld * matViewProj);

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
	bool		isLineMode = false;

	Matrix		matViewProj;
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
