#include "Material.h"
#include "ShaderProgramMgr.h"
#include "TextureMgr.h"

Material::Material()
{
}


Material::~Material()
{
}

bool Material::loadShader(const std::string &path)
{
	shader_ = ShaderProgramMgr::instance()->get(path);
	return shader_;
}

bool Material::loadTexture(const std::string & key, const std::string & path)
{
	TexturePtr texture = TextureMgr::instance()->get(path);
	textures_[key] = texture;

	return texture;
}

void Material::bindShader()
{
	shader_->bind();
}

void Material::setTexture(const std::string & name, TexturePtr texture)
{
	textures_[name] = texture;
}

TexturePtr Material::getTexture(const std::string & key)
{
	auto it = textures_.find(key);
	if (it != textures_.end())
	{
		return it->second;
	}
	return nullptr;
}

bool Material::begin()
{
	if (!shader_)
	{
		return false;
	}

	shader_->bind();
	for (auto &pair : textures_)
	{
		bindUniform(pair.first, pair.second.get());
	}
	return true;
}

void Material::end()
{
	shader_->unbind();
}
