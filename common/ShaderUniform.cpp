#include "ShaderUniform.h"
#include "ShaderProram.h"
#include "Color.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix.h"

#include "LogTool.h"


ShaderUniform::ShaderUniform(const std::string & name)
    : name_(name)
    , location_(-1)
    , type_(0)
    , index_(0)
    , pEffect_(nullptr)
{
}

ShaderUniform::~ShaderUniform()
{
    for(auto it : children_)
    {
        delete it.second;
    }
}

Effect* ShaderUniform::getEffect() const
{
    return pEffect_;
}

const std::string& ShaderUniform::getName() const
{
    return name_;
}

const GLenum ShaderUniform::getType() const
{
    return type_;
}

ShaderUniform *ShaderUniform::getChild(const std::string & name, bool createIfMiss)
{
    auto it = children_.find(name);
    if(it != children_.end()) return it->second;

    if(createIfMiss)
    {
        ShaderUniform *p = new ShaderUniform(name);
        p->pEffect_ = pEffect_;

        children_[name] = p;
        return p;
    }

    return nullptr;
}

ShaderUniform *ShaderUniform::getChildren(const std::string & name, bool createIfMiss)
{
    ASSERT_2(!name.empty(), "ShaderUniform::getChild");

    size_t pos = name.find('.');
    if(pos == name.npos)
    {
        return getChild(name, createIfMiss);
    }

    ShaderUniform *pChild = getChild(name.substr(0, pos), createIfMiss);
    if(!pChild) return nullptr;

    return pChild->getChildren(name.substr(pos + 1), createIfMiss);
}


void ShaderUniform::bindValue(float value)
{
    GL_ASSERT( glUniform1f(location_, value) );
}

void ShaderUniform::bindValue(const float* values, uint32 count)
{
    GL_ASSERT( glUniform1fv(location_, count, values) );
}

void ShaderUniform::bindValue(int value)
{
    GL_ASSERT( glUniform1i(location_, value) );
}

void ShaderUniform::bindValue(const int* values, uint32 count)
{
    GL_ASSERT( glUniform1iv(location_, count, values) );
}

void ShaderUniform::bindValue(const Matrix& value)
{
    Matrix mat;
    mat.transpose(value);
    GL_ASSERT(glUniformMatrix4fv(location_, 1, GL_FALSE, (const float*) (&mat)));
}

void ShaderUniform::bindValue(const Matrix* values, uint32 count, bool transposed)
{
    ASSERT_1(values && count > 0);

    if(transposed)
    {
        glUniformMatrix4fv(location_, count, GL_FALSE, (GLfloat*) values);
    }
    else
    {
        Matrix * pBuffer = new Matrix[count];
        for (uint32 i = 0; i < count; ++i)
            pBuffer[i].transpose(values[i]);

        glUniformMatrix4fv(location_, count, GL_FALSE, (GLfloat*) pBuffer);

        delete [] pBuffer;
    }
}

void ShaderUniform::bindValue(const Vector2& value)
{
    GL_ASSERT( glUniform2f(location_, value.x, value.y) );
}

void ShaderUniform::bindValue(const Vector2* values, uint32 count)
{
    GL_ASSERT( glUniform2fv(location_, count, (GLfloat*)values) );
}

void ShaderUniform::bindValue(const Vector3& value)
{
    GL_ASSERT( glUniform3f(location_, value.x, value.y, value.z) );
}

void ShaderUniform::bindValue(const Vector3* values, uint32 count)
{
    GL_ASSERT( glUniform3fv(location_, count, (GLfloat*)values) );
}

void ShaderUniform::bindValue(const Vector4& value)
{
    GL_ASSERT( glUniform4f(location_, value.x, value.y, value.z, value.w) );
}

void ShaderUniform::bindValue(const Vector4* values, uint32 count)
{
    GL_ASSERT( glUniform4fv(location_, count, (GLfloat*)values) );
}

void ShaderUniform::bindValue(const Color & color)
{
    GL_ASSERT( glUniform4f(location_, color.r, color.g, color.b, color.a) );
}

void ShaderUniform::bindValue(TexturePtr texture)
{
    ASSERT_1(type_ == GL_SAMPLER_2D);

    texture_ = texture;

    //binds the texture
    GL_ASSERT(glActiveTexture(GL_TEXTURE0 + index_));

    if (texture)
    {
        texture->bind();
    }
    else
    {
        GL_ASSERT( glBindTexture(GL_TEXTURE_2D, 0) );
    }
    
    GL_ASSERT( glUniform1i(location_, index_) );
}

//////////////////////////////////////////////////////////////////

/*static*/ std::map<std::string, ShaderAutoUniform*> ShaderAutoUniform::s_autoConstMap;

/*static*/ ShaderAutoUniform * ShaderAutoUniform::get(const std::string & name)
{
    auto it = s_autoConstMap.find(name);
    if (it != s_autoConstMap.end()) return it->second;

    return nullptr;
}

/*static*/ void ShaderAutoUniform::set(const std::string & name, ShaderAutoUniform * autoConst)
{
    s_autoConstMap.insert(std::make_pair(name, autoConst));
}

/*static*/ void ShaderAutoUniform::fini()
{
    for (auto it = s_autoConstMap.begin(); it != s_autoConstMap.end(); ++it)
    {
        delete it->second;
    }
    s_autoConstMap.clear();
}


ShaderAutoUniform::ShaderAutoUniform()
{

}

ShaderAutoUniform::~ShaderAutoUniform()
{

}
