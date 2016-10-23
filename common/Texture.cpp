#include "Texture.h"
#include "LogTool.h"

#include "stb/stb_image.h"
#include "stb/stb_image_write.h"


int g_texture_counter = 0;

Texture::Texture()
    : format_(TextureFormat::Unknown)
    , handle_(0)
    , width_(0)
    , height_(0)
    , mipmapped_(0)
    , parameterDirty_(1)
    , uwrap_(TextureWrap::Clamp)
    , vwrap_(TextureWrap::Clamp)
    , target_(TextureTarget::Tex2D)
    , quality_(TextureQuality::Default)
    , pLoadingInfo_(nullptr)
{
    ++g_texture_counter;
}

Texture::~Texture()
{
    --g_texture_counter;
    
    destroy();
}

void Texture::destroy()
{
    if (handle_ != 0)
    {
        tryUnbind();

        glDeleteTextures(1, &handle_);
        handle_ = 0;
    }

    if(pLoadingInfo_ != nullptr)
    {
        delete [] pLoadingInfo_->pPixelData;
        delete pLoadingInfo_;
        pLoadingInfo_ = nullptr;
    }

    mipmapped_ = 0;
    parameterDirty_ = 1;
}

bool Texture::load(const std::string & filename)
{
    resource_ = filename;

    std::string buffer;
    do
    {
        if (readFile(buffer, filename, true))
            break;
    
        if(stringEndWith(filename, ".bmp") || stringEndWith(filename, ".tga"))
        {
            if(readFile(buffer, removeFileExt(filename) + ".tga", true))
                break;
        }

        ORA_LOG_ERROR("Failed to open texture file '%s'", filename.c_str());
        return false;
    }while(0);

    int w, h, comp;
    stbi_uc * pixelData = stbi_load_from_memory((stbi_uc*) &buffer[0], buffer.size(), &w, &h, &comp, 0);
    if (!pixelData)
    {
        ORA_LOG_ERROR("Failed to load texture '%s'", filename.c_str());
        return false;
    }

    TextureFormat format = TextureFormat::Unknown;
    switch (comp)
    {
    case STBI_grey:
        format = TextureFormat::Luminance;
        break;

    case STBI_grey_alpha:
        format = TextureFormat::LuminanceAlpha;
        break;

    case STBI_rgb:
        format = TextureFormat::RGB;
        break;

    case STBI_rgb_alpha:
        format = TextureFormat::RGBA;
        break;
    default:
        break;
    }

    bool ret = false;
    if (format != TextureFormat::Unknown)
    {
        ret = create(0, w, h, format, pixelData, w * h * comp);
    }

    stbi_image_free(pixelData);
    return ret;
}

void Texture::loadingImmediately()
{
    if(pLoadingInfo_ != nullptr)
    {
        doLoading();
    }
}

void Texture::doLoading()
{
    ASSERT_1(pLoadingInfo_ != nullptr);

    GLenum internalFormat = GLenum(format_);
    GLenum type = GL_UNSIGNED_BYTE;

    GL_ASSERT(glGenTextures(1, &handle_));

    if (pLoadingInfo_->pPixelData)
    {
        GL_ASSERT(glBindTexture(GL_TEXTURE_2D, handle_));
        GL_ASSERT(glTexImage2D(GL_TEXTURE_2D,
            pLoadingInfo_->levels,
            internalFormat,
            width_,
            height_,
            0, 
            (GLenum) format_,
            type,
            pLoadingInfo_->pPixelData));

        delete [] pLoadingInfo_->pPixelData;
    }

    delete pLoadingInfo_;
    pLoadingInfo_ = nullptr;
}

bool Texture::save(const std::string & filename)
{
    int saveChannels = 4;
    GLenum saveFormat = GL_RGBA;

    int imageSize = width_ * height_ * saveChannels;
    char *pData = new char[imageSize];

    int oldAligment;
    glGetIntegerv(GL_PACK_ALIGNMENT, &oldAligment);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glBindTexture(GL_TEXTURE_2D, handle_);
    glReadPixels(0, 0, width_, height_, saveFormat, GL_UNSIGNED_BYTE, pData);// split x and y sizes into bytes
    if (GLenum error = glGetError())
        ORA_LOG_ERROR("OpenGLTexture::read pixels failed! %x", error);
    glPixelStorei(GL_PACK_ALIGNMENT, oldAligment);

    glBindTexture(GL_TEXTURE_2D, 0);

    int ret = 0;
    std::string fullpath = FileSystemMgr::instance()->getWritablePath() + filename;
    ret = stbi_write_tga(fullpath.c_str(), width_, height_, saveChannels, pData);
    
    delete pData;
    return ret != 0;
}

bool Texture::create(uint32 levels, uint32 width, uint32 height, TextureFormat format, const void* pPixelData, size_t size)
{
    ASSERT_1(width != 0 && height != 0);
    ASSERT_1(format != TextureFormat::Unknown);

    destroy();

    width_ = width;
    height_ = height;
    format_ = format;

    TextureLoadingInfo * pInfo = new TextureLoadingInfo();
    pInfo->levels = levels;
    pInfo->size = size;
    pInfo->pPixelData = nullptr;
    if(pPixelData != nullptr)
    {
        pInfo->pPixelData = new char[size];
        memcpy(pInfo->pPixelData, pPixelData, size);
    }

    pLoadingInfo_ = pInfo;
    return true;
}

bool Texture::create(GLuint handle, uint32 width, uint32 height, TextureFormat format)
{
    destroy();

    handle_ = handle;
    width_ = width;
    height_ = height;
    format_ = format;
    return true;
}

void Texture::setUWrap(TextureWrap wrap)
{
    if (wrap == uwrap_) return;

    uwrap_ = wrap;
    parameterDirty_ = 1;
}

void Texture::setVWrap(TextureWrap wrap)
{
    if (wrap == vwrap_) return;

    vwrap_ = wrap;
    parameterDirty_ = 1;
}

void Texture::setQuality(TextureQuality quality)
{
    if (quality == quality_) return;

    quality_ = quality;
    parameterDirty_ = 1;
}

void Texture::generateMipmaps()
{
    if (handle_ == 0 || mipmapped_) return;

    ASSERT_2(target_ == TextureTarget::Tex2D, "this format does't supported rightnow!");

    mipmapped_ = 1;
    GL_ASSERT(glGenerateMipmap(GL_TEXTURE_2D));
}

void Texture::updateParameter()
{
    if (!parameterDirty_ || !handle_) return;
    parameterDirty_ = 1;

    GLenum target = GLenum(target_);

#ifndef _RELEASE

    GLint handle = 0;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &handle);
    ASSERT_1(handle == handle_);

#endif

    TextureQuality quality = quality_;
    if (quality == TextureQuality::Default)
        quality = RenderConfig::instance()->getTextureQuality();

    if ((width_ & (width_ - 1)) != 0 ||
        (height_ & (height_ - 1)) != 0)
    {
        if (int(quality) > int(TextureQuality::TwoLinear))
            quality = TextureQuality::TwoLinear;
    }

    switch (quality)
    {
    case TextureQuality::Nearest:
        glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        break;

    case TextureQuality::Linear:
        glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        break;

    case TextureQuality::TwoLinear:
        glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        break;

    case TextureQuality::ThreeLinear:
        generateMipmaps();
        glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        break;

    case TextureQuality::FourLinear:
    case TextureQuality::Anisotropic:
        generateMipmaps();
        glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        break;

    default:
        break;
    };

    glTexParameteri(target, GL_TEXTURE_WRAP_S, GLenum(uwrap_));
    glTexParameteri(target, GL_TEXTURE_WRAP_T, GLenum(vwrap_));
}

void Texture::onDeviceClose()
{
    destroy();
}


bool Texture::bind()
{
    if(pLoadingInfo_ != nullptr)
    {
        doLoading();
    }

    glBindTexture(GL_TEXTURE_2D, handle_);

    if(handle_ == 0)
    {
        return false;
    }

    updateParameter();
    return true;
}

void Texture::unbind()
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

bool Texture::tryUnbind()
{
    GLint curTexture;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &curTexture);
    if (handle_ == curTexture)
    {
        glBindTexture(GL_TEXTURE_2D, 0);
        return true;
    }

    return false;
}

////////////////////////////////////////////////////////////////////
TextureStage::TextureStage()
    : uWrap_(TextureWrap::Clamp)
    , vWrap_(TextureWrap::Clamp)
{

}

TextureStage::~TextureStage()
{

}

void TextureStage::applyParam()
{
    if(texture_)
    {
        texture_->setUWrap(uWrap_);
        texture_->setVWrap(vWrap_);
    }
}
