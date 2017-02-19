#include "Application.h"
#include "ShaderProgram.h"
#include "ShaderProgramMgr.h"
#include "ShaderUniform.h"
#include "TextureMgr.h"
#include "FileSystem.h"
#include "DemoTool.h"
#include "PathTool.h"
#include "LogTool.h"
#include "Matrix.h"
#include "Mesh.h"
#include "Camera.h"
#include "Model.h"
#include "Renderer.h"
#include "title.h"
#include "Vertex.h"
#include "VertexBuffer.h"

class MyApplication : public Application
{
public:

	MyApplication()
	{
		glfwWindowHint(GLFW_SAMPLES, 4);
	}

	bool onCreate() override
	{
		std::string resPath = findResPath();
		FileSystem::instance()->addSearchPath(resPath);
		FileSystem::instance()->addSearchPath(joinPath(resPath, "common"));
		FileSystem::instance()->dumpSearchPath();

		modelShader_ = ShaderProgramMgr::instance()->get("shader/model.shader");
		if (!modelShader_)
		{
			return false;
		}

		Vector3 lightDir(1, 1, 0);
		lightDir.normalize();

		modelShader_->bind();
		bindShaderUniform(modelShader_.get(), "lightDir", lightDir);
		bindShaderUniform(modelShader_.get(), "lightColor", Vector3(1.5f));
		bindShaderUniform(modelShader_.get(), "ambientColor", Vector3(0.5f));
		modelShader_->unbind();

		model_ = new Model();
		if (!model_->load("model/axe.x", modelShader_))
		{
			return false;
		}
		modelTransform_.setScale(0.1f);

		camera_.lookAt(Vector3(0, 1, -2), Vector3::Zero, Vector3::YAxis);
		setupViewProjMatrix();

		glDisable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		// ����ģ�����
		glEnable(GL_STENCIL_TEST);

		quadShader_ = ShaderProgramMgr::instance()->get("shader/xyzcolor.shader");
		if (!quadShader_)
		{
			return false;
		}

		VertexXYZColor vertices[4] = {
			{ { -0.5f, 0.5f, 0.0f },{ 0.0f, 1.0f, 0.0f, 0.5f } }, // left top
			{ { -0.5f, -0.5f, 0.0f },{ 1.0f, 0.0f, 0.0f, 0.5f } }, // left bottom
			{ { 0.5f, 0.5f, 0.0f },{ 0.0f, 0.0f, 1.0f, 0.5f } }, // right top
			{ { 0.5f, -0.5f, 0.0f },{ 1.0f, 0.0f, 0.0f, 0.5f } }, // right bottom
		};
		VertexBufferPtr quadVB_ = new VertexBufferEx<VertexXYZColor>(BufferUsage::Static, 4, vertices);
		VertexDeclarationPtr decl = VertexDeclMgr::instance()->get(VertexXYZColor::getType());
		quadVAO_ = new VertexAttribute();
		quadVAO_->init(quadVB_.get(), decl.get());

		return true;
	}

	void onTick() override
	{
		camera_.handleCameraMove();
	}

	void onDraw() override
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		Renderer::instance()->setViewMatrix(camera_.getViewMatrix());
		Renderer::instance()->setProjMatrix(camera_.getProjMatrix());

		// �Ѿ��θ��ǵ�����ģ��ֵ����Ϊ 1
		glStencilFunc(GL_ALWAYS, 0, 0xffffffff);
		glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);

		// �ر���ɫд������д�롣Ŀ����ֻ�ı�ģ���ֵ������Ӱ��ԭ�������ֵ����ɫֵ��
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glDepthMask(GL_FALSE);
		drawQuad();

		// �ָ���ɫд������д��
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glDepthMask(GL_TRUE);

		// ֻ��������ֵ(0) < ģ��ֵ��������Խ�����Ⱦ
		glStencilFunc(GL_LESS, 0, 0xffffffff);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

		model_->applyMatrix(modelTransform_.getModelMatrix());
		model_->draw();

		// �ھ��α�����Ⱦһ���͸��ͼ��
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		drawQuad();
		glDisable(GL_BLEND);
	}

	void drawQuad()
	{
		Transform transform;
		transform.setPosition(-0.5f, 0.0f, -0.5f);
		transform.setScale(1.0f);

		auto renderer = Renderer::instance();
		renderer->setWorldMatrix(transform.getModelMatrix());

		quadShader_->bind();
		quadShader_->applyAutoUniforms();
		quadVAO_->bind();
		
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		quadVAO_->unbind();
		quadShader_->unbind();
	}

	void onSizeChange(int width, int height) override
	{
		Application::onSizeChange(width, height);
		setupViewProjMatrix();
	}

	void setupViewProjMatrix()
	{
		Vector2 size = getWindowSize();
		camera_.setPerspective(PI_QUARTER, size.x / size.y, 1.0f, 1000.0f);
	}
	
	virtual void onMouseButton(int button, int action, int mods) override
	{
		lastCursorPos_ = getCursorPos();
		camera_.handleMouseButton(button, action, mods);
	}

	virtual void onMouseMove(double x, double y) override
	{
		if (gApp->isMousePress(GLFW_MOUSE_BUTTON_LEFT))
		{
			Vector2 size = gApp->getWindowSize();

			float dx = (float(x) - lastCursorPos_.x) / size.x;
			float dy = (float(y) - lastCursorPos_.y) / size.y;

			Vector3 rotation = modelTransform_.getRotation();
			rotation.y -= dx * PI_FULL;
			rotation.x -= dy * PI_FULL;
			modelTransform_.setRotation(rotation);
		}
		else
		{
			camera_.handleMouseMove(x, y);
		}

		lastCursorPos_.set(x, y);
	}

	virtual void onMouseScroll(double xoffset, double yoffset) override
	{
		camera_.handleMouseScroll(xoffset, yoffset);
	}

	ShaderProgramPtr modelShader_;
	Camera		camera_;
	ModelPtr	model_;
	Vector2		lastCursorPos_;
	Transform	modelTransform_;

	ShaderProgramPtr quadShader_;
	VertexAttributePtr quadVAO_;
};

int main()
{
    MyApplication app;
    if(app.createWindow(800, 600, APP_TITLE))
    {
        app.mainLoop();
    }
    return 0;
}
