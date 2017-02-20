#pragma once

#include <unordered_map>
#include "Texture.h"
#include "ShaderProgram.h"
#include "ShaderUniform.h"

class Material : public ReferenceCount
{
public:
	Material();
	~Material();

	void setShader(ShaderProgramPtr shader) { shader_ = shader; }
	ShaderProgramPtr getShader() { return shader_; }

	void setTexture(const std::string &key, TexturePtr texture);
	TexturePtr getTexture(const std::string &key);

	bool begin();
	void end();

	template<typename T>
	bool bindUniform(const std::string &name, const T &value);

	bool loadShader(const std::string &path);
	TexturePtr loadTexture(const std::string & key, const std::string &path);
	void bindShader();

	void setAutoBindUniform(bool enable) { autoBindUniform_ = enable; }

private:
	ShaderProgramPtr shader_;
	std::unordered_map<std::string, TexturePtr> textures_;
	bool	autoBindUniform_;
};

typedef SmartPointer<Material> MaterialPtr;

template<typename T>
bool Material::bindUniform(const std::string &name, const T &value)
{
	ShaderUniform *un = shader_->findUniform(name);
	if (un != nullptr)
	{
		un->bindValue(value);
		return true;
	}
	return false;
}
