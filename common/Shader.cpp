#include "Shader.h"
#include "FileSystem.h"
#include "LogTool.h"
#include "glconfig.h"

Shader::Shader(uint32_t type)
: handle_(0)
, type_(type)
{

}

Shader::~Shader()
{
    if(glIsShader(handle_))
    {
        glDeleteShader(handle_);
    }
}

bool Shader::loadFromFile(const std::string &fileName)
{
    fileName_ = fileName;

    std::string data;
    if(!FileSystem::instance()->readFile(data, fileName))
    {
        return false;
    }

    return loadFromData(data);
}

bool Shader::loadFromData(const std::string &data)
{
    handle_ = glCreateShader(type_);
    if(!glIsShader(handle_))
    {
        return false;
    }

    const char *code = data.c_str();
    glShaderSource(handle_, 1, &code, nullptr);
    glCompileShader(handle_);

    GLint params;
    glGetShaderiv(handle_, GL_COMPILE_STATUS, &params);
    if(params != GL_TRUE)
    {
        LOG_ERROR("Failed to compile shader: %s", getCompileError().c_str());
        return false;
    }
    return true;
}

std::string Shader::getCompileError() const
{
    GLint logLength;
    glGetShaderiv(handle_, GL_INFO_LOG_LENGTH, &logLength);

    std::string str(logLength, 0);
    glGetShaderInfoLog(handle_, logLength, nullptr, const_cast<char*>(str.data()));
    return str;
}
