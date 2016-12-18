#include "Texture.h"
#include "LogTool.h"
#include "PathTool.h"
#include "FileSystem.h"

#include "stb/stb_image.h"
#include "stb/stb_image_write.h"


int g_texture_counter = 0;
#ifdef __APPLE__
TextureQuality Texture::s_defaultQuality = TextureQuality::TwoLinear;
#else
TextureQuality Texture::s_defaultQuality = TextureQuality::ThreeLinear;
#endif

Texture::Texture()
    : format_(TextureFormat::Unknown)
    , handle_(0)
    , width_(0)
    , height_(0)
    , mipmapped_(false)
    , parameterDirty_(true)
    , uwrap_(TextureWrap::Clamp)
    , vwrap_(TextureWrap::Clamp)
    , target_(TextureTarget::Tex2D)
    , quality_(TextureQuality::Default)
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
    if (glIsTexture(handle_))
    {
        tryUnbind();

        glDeleteTextures(1, &handle_);
        handle_ = 0;
    }

    mipmapped_ = false;
    parameterDirty_ = true;
}

bool Texture::load(const std::string & filename)
{
    std::string buffer;
    if (!FileSystem::instance()->readFile(buffer, filename, true))
    {
        LOG_ERROR("Failed to open texture file '%s'", filename.c_str());
        return false;
    }
    
    resource_ = filename;

    int w, h, comp;
    stbi_uc * pixelData = stbi_load_from_memory((stbi_uc*) &buffer[0], buffer.size(), &w, &h, &comp, 0);
    if (!pixelData)
    {
        LOG_ERROR("Failed to load texture '%s'", filename.c_str());
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
        ret = create(0, w, h, format, pixelData, GL_UNSIGNED_BYTE);
    }

    stbi_image_free(pixelData);
    return ret;
}

bool Texture::save(const std::string & filename) const
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
    {
        LOG_ERROR("OpenGLTexture::read pixels failed! %x", error);
    }
    glPixelStorei(GL_PACK_ALIGNMENT, oldAligment);

    glBindTexture(GL_TEXTURE_2D, 0);

    std::string fullpath = FileSystem::instance()->resolveWritablePath(filename);
    int ret = stbi_write_tga(fullpath.c_str(), width_, height_, saveChannels, pData);
    
    delete []pData;
    return ret != 0;
}

bool Texture::create(uint32_t levels, uint32_t width, uint32_t height, TextureFormat format, const void* pPixelData, uint32_t pxieType)
{
    assert(width != 0 && height != 0);
    assert(format != TextureFormat::Unknown);

    destroy();

    width_ = width;
    height_ = height;
    format_ = format;
    
    GLenum internalFormat = GLenum(format_);
    
	GL_ASSERT(glGenTextures(1, &handle_));
	GL_ASSERT(glBindTexture(GL_TEXTURE_2D, handle_));

	int oldAlignment;
	glGetIntegerv(GL_PACK_ALIGNMENT, &oldAlignment);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);

	GL_ASSERT(glTexImage2D(GL_TEXTURE_2D, levels, internalFormat, width_, height_,
		0, internalFormat, pxieType, pPixelData));

	glPixelStorei(GL_PACK_ALIGNMENT, oldAlignment);
    return true;
}

bool Texture::create(GLuint handle, uint32_t width, uint32_t height, TextureFormat format)
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
    parameterDirty_ = true;
}

void Texture::setVWrap(TextureWrap wrap)
{
    if (wrap == vwrap_) return;

    vwrap_ = wrap;
    parameterDirty_ = true;
}

void Texture::setQuality(TextureQuality quality)
{
    if (quality == quality_) return;

    quality_ = quality;
    parameterDirty_ = true;
}

void Texture::generateMipmaps()
{
    if (handle_ == 0 || mipmapped_) return;

    assert(target_ == TextureTarget::Tex2D && "this format does't supported rightnow!");

    mipmapped_ = true;
    if(glGenerateMipmap != nullptr)
    {
        GL_ASSERT(glGenerateMipmap(GL_TEXTURE_2D));
    }
}

void Texture::updateParameter()
{
    if (!parameterDirty_ || !handle_) return;
    parameterDirty_ = false;

    GLenum target = GLenum(target_);

#ifdef DEBUG 
    GLint handle = 0;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &handle);
    assert(handle == handle_);
#endif

    TextureQuality quality = quality_;
    if (quality == TextureQuality::Default)
        quality = s_defaultQuality;

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

bool Texture::bind()
{
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

