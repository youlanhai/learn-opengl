#ifndef H__TEXTURE_H
#define H__TEXTURE_H

#include "Reference.h"
#include "SmartPointer.h"
#include "RenderState.h"
#include <string>

typedef SmartPointer<class Texture> TexturePtr;

class Texture : public ReferenceCount
{
public:
    Texture();
    virtual ~Texture();

    virtual bool load(const std::string & filename);
    virtual bool save(const std::string & filename) const;

    bool create(uint32_t levels, uint32_t width, uint32_t height, TextureFormat format, const void* pPixelData, uint32_t pxieType);
    bool create(GLuint handle, uint32_t width, uint32_t height, TextureFormat format);

    void setUWrap(TextureWrap wrap);
    TextureWrap getUWrap() const{ return uwrap_; }

    void setVWrap(TextureWrap wrap);
    TextureWrap getVWrap() const { return vwrap_; }

    void setQuality(TextureQuality quality);
    TextureQuality getQuality() const { return quality_; }

    const std::string & getResource() const {  return resource_; }
    TextureFormat getFormat() const { return format_; }
    uint32_t getWidth() const { return width_; }
    uint32_t getHeight() const { return height_; }
    bool   isMipmapped() const { return mipmapped_ != 0; }
    GLuint getHandle() const { return handle_; }

    bool bind();
    // 强制将当前GL纹理设置为0
    void unbind();
    // 如果当前GL纹理是自己，将其设置为0，并返回true。否则返回false。
    // 当纹理对象析构的时候，可能需要此函数。
    bool tryUnbind();

protected:
    
    void destroy();

    virtual void updateParameter();
    virtual void generateMipmaps();

    std::string         resource_;
    TextureFormat       format_;
    GLuint              handle_;
    uint32_t            width_;
    uint32_t            height_;
    bool                mipmapped_;
    bool                parameterDirty_;
    TextureWrap         uwrap_;
    TextureWrap         vwrap_;
    TextureTarget       target_;
    TextureQuality      quality_;
    
public:
    static TextureQuality s_defaultQuality;
};

#endif //H__TEXTURE_H
