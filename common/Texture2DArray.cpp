#include "Texture2DArray.h"
#include "LogTool.h"

Texture2DArray::Texture2DArray()
{
    target_ = TextureTarget::Tex2DArray;
}

Texture2DArray::~Texture2DArray()
{
    
}

bool Texture2DArray::load(const std::string & fileName)
{
    return false;
}

bool Texture2DArray::save(const std::string & fileName) const
{
    return false;
}

bool Texture2DArray::create(int levels, uint32_t width, uint32_t height, TextureFormat format, int layerCount)
{
    destroy();
    
    width_ = width;
    height_ = height;
    format_ = format;
    layerCount_ = layerCount;
    
    glGenTextures(1, &handle_);
    glBindTexture((GLenum)target_, handle_);
    GL_ASSERT(glTexImage3D((GLenum)target_, levels, (GLenum)format, width, height, layerCount, 0, (GLenum)format, GL_UNSIGNED_BYTE, 0));
    return true;
}
