#include "TextureCube.h"
#include "glconfig.h"
#include "FileSystem.h"
#include "LogTool.h"
#include "PathTool.h"

#include <stb/stb_image.h>
#include <stb/stb_image_write.h>
#include <smartjson/smartjson.hpp>

TextureCube::TextureCube()
{
	target_ = TextureTarget::TexCubeMap;
}

TextureCube::~TextureCube()
{
}

bool TextureCube::load(const std::string & fileName)
{
	destroy();

	std::string buffer;
	if (!FileSystem::instance()->readFile(buffer, fileName))
	{
		LOG_ERROR("Failed to open texture file '%s'", fileName.c_str());
		return false;
	}

	mjson::Parser parser;
	if (!parser.parseFromData(buffer.c_str(), buffer.size()))
	{
		LOG_ERROR("Failed parse json: %s : error %d", fileName.c_str(), parser.getErrorCode());
		return false;
	}

	mjson::Node root = parser.getRoot();
	if (root["type"] != "TextureCube")
	{
		LOG_ERROR("Invalid file format for cube texture.");
		return false;
	}

	resource_ = fileName;
	std::string fileDir = getFilePath(fileName);

	GL_ASSERT(glGenTextures(1, &handle_));
	GL_ASSERT(glBindTexture((GLenum)target_, handle_));

	int oldAlignment;
	glGetIntegerv(GL_PACK_ALIGNMENT, &oldAlignment);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);

	static const char *keys[] = {"right", "left", "up", "down", "back", "front", };
	bool ret = true;

	format_ = TextureFormat::Unknown;

	for (int i = 0; i < 6 && ret; ++i)
	{
		ret = false;

		const char *key = keys[i];
		std::string path = root[key].asStdString();
		if (path.empty())
		{
			LOG_ERROR("Failed find texture for '%s'", key);
			break;
		}

		path = joinPath(fileDir, path);
		if (!FileSystem::instance()->readFile(buffer, path, true))
		{
			LOG_ERROR("Failed to open texture file '%s'", path.c_str());
			break;
		}

		int w, h, comp;
		stbi_uc * pixelData = stbi_load_from_memory((stbi_uc*)&buffer[0], buffer.size(), &w, &h, &comp, 0);
		if (!pixelData)
		{
			LOG_ERROR("Failed to load texture '%s'", path.c_str());
			break;
		}

		TextureFormat format = component2format(comp);
		if (format_ == TextureFormat::Unknown) format_ = format;
		if (width_ == 0) width_ = w;
		if (height_ == 0) height_ = h;

		if (width_ != w || height_ != h)
		{
			LOG_ERROR("The size of cube texture '%s' was not equal to others.", path.c_str());
			break;
		}
		if (format_ != format)
		{
			LOG_ERROR("The format of cube texture '%s' was not equal to others", path.c_str());
			break;
		}
		
		if (format_ != TextureFormat::Unknown)
		{
			GLenum internalFormat = GLenum(format_);
			GL_ASSERT(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, internalFormat, w, h,
				0, internalFormat, GL_UNSIGNED_BYTE, pixelData));
			ret = true;
		}

		stbi_image_free(pixelData);
	}

	glPixelStorei(GL_PACK_ALIGNMENT, oldAlignment);
	return ret;
}

bool TextureCube::save(const std::string & filename) const
{
	return false;
}

bool TextureCube::create(uint32_t levels, uint32_t width, uint32_t height, TextureFormat format, const void* pPixelData, uint32_t pxieType)
{
	return false;
}
