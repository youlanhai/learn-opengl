#pragma once

#include "Texture.h"

// 立方体纹理
class TextureCube : public Texture
{
public:
	TextureCube();
	~TextureCube();

	/** 文件是以.cube为后缀的json文件。
	*	type = TextureCube。
	*	具备 "right", "left", "up", "down", "back", "front", 6张贴图，
	*	贴图的路径必须相对与fileName所在的路径。
	*/
	virtual bool load(const std::string & fileName);

	/** 暂不支持保存 */
	virtual bool save(const std::string & fileName) const;

	/** 不支持此函数 */
	virtual bool create(uint32_t levels, uint32_t width, uint32_t height, TextureFormat format, const void* pPixelData, uint32_t pxieType);
};
