#include "ShaderProgram.h"
#include "Shader.h"
#include "FileSystem.h"
#include "LogTool.h"
#include "glconfig.h"

#include <smartjson/sj_parser.hpp>
#include <iostream>

ShaderProgram::ShaderProgram()
: handle_(0)
{

}

ShaderProgram::~ShaderProgram()
{
    if(glIsProgram(handle_))
    {
        glDeleteProgram(handle_);
    }
}

bool ShaderProgram::loadFromFile(const std::string &fileName)
{
    fileName_ = fileName;
    
    std::string data;
    if(!FileSystem::instance()->readFile(data, fileName))
    {
        return false;
    }
    
    return loadFromData(data);
}

bool ShaderProgram::loadFromData(const std::string &data)
{
    mjson::Parser parser;
    if(!parser.parseFromData(data.c_str(), data.size()))
    {
        LOG_ERROR("Failed parse json: %s : error %d", fileName_.c_str(), parser.getErrorCode());
        return false;
    }
    
    mjson::Node root = parser.getRoot();
    
    Shader vs(GL_VERTEX_SHADER);
    if(!vs.loadFromFile(root["vertexShader"].asStdString()))
    {
        return false;
    }
    
    Shader fs(GL_FRAGMENT_SHADER);
    if(!fs.loadFromFile(root["fragmentShader"].asStdString()))
    {
        return false;
    }
    
    handle_ = glCreateProgram();
    if(!glIsProgram(handle_))
    {
        LOG_ERROR("Failed to create shader prgram");
        return false;
    }
    
    glAttachShader(handle_, vs.getHandle());
    glAttachShader(handle_, fs.getHandle());
    glLinkProgram(handle_);

	glDetachShader(handle_, vs.getHandle());
	glDetachShader(handle_, fs.getHandle());
    
    GLint status;
    glGetProgramiv(handle_, GL_LINK_STATUS, &status);
    if(status != GL_TRUE)
    {
        LOG_ERROR("Failed to link shader program: %s", getLinkError().c_str());
        return false;
    }
    
    return true;
}

std::string ShaderProgram::getLinkError() const
{
    GLint length;
    glGetProgramiv(handle_, GL_INFO_LOG_LENGTH, &length);
    
    std::string ret;
    glGetProgramInfoLog(handle_, length, nullptr, const_cast<char*>(ret.data()));
   
    return ret;
}

void ShaderProgram::use()
{
    glUseProgram(handle_);
}

int ShaderProgram::getUniformLocation(const char *name)
{
    return glGetUniformLocation(handle_, name);
}

int ShaderProgram::getAttribLocation(const char *name)
{
    return glGetAttribLocation(handle_, name);
}

void ShaderProgram::setMatrix(int location, const float *data)
{
	glProgramUniformMatrix4fv(handle_, location, 1, false, data);
}

void ShaderProgram::setMatrixTranspose(int location, const float * data)
{
	glProgramUniformMatrix4fv(handle_, location, 1, true, data);
}
