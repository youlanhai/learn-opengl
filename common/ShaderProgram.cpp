#include "ShaderProgram.h"
#include "Shader.h"
#include "FileSystem.h"
#include "LogTool.h"
#include "glconfig.h"
#include "ShaderUniform.h"
#include "PathTool.h"

#include <smartjson/sj_parser.hpp>
#include <iostream>

ShaderProgram::ShaderProgram()
: handle_(0)
, uniformRoot_(new ShaderUniform("root"))
{
    for(int i = 0; i < VertexUsageMax; ++i)
    {
        attributes_[i] = -1;
    }
}

ShaderProgram::~ShaderProgram()
{
	delete uniformRoot_;

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

	std::string rootPath = getFilePath(fileName_);
    
    Shader vs(GL_VERTEX_SHADER);
	std::string path = joinPath(rootPath, root["vertexShader"].asStdString());
    if(!vs.loadFromFile(path))
    {
        return false;
    }
    
    Shader fs(GL_FRAGMENT_SHADER);
	path = joinPath(rootPath, root["fragmentShader"].asStdString());
    if(!fs.loadFromFile(path))
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

	if (!parseAttributes())
	{
		return false;
	}
	if (!parseUniforms())
	{
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

bool ShaderProgram::parseAttributes()
{
	GLint numAttributes;
	GL_ASSERT(glGetProgramiv(handle_, GL_ACTIVE_ATTRIBUTES, &numAttributes));
	if (numAttributes <= 0)
	{
		return false;
	}

	int maxLength;
	GL_ASSERT(glGetProgramiv(handle_, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxLength));
	if (maxLength <= 0)
	{
		return false;
	}

	std::string attribName;
	GLint   attribSize;
	GLenum  attribType;
	GLint   attribLocation;
	GLint   length;
	for (GLint i = 0; i < numAttributes; ++i)
	{
		attribName.resize(maxLength);
		glGetActiveAttrib(handle_, i, maxLength, &length, &attribSize, &attribType, &attribName[0]);
		if (length < attribName.size())
		{
			attribName.erase(length);
		}

		attribLocation = glGetAttribLocation(handle_, attribName.c_str());

		VertexUsage usage = vertexAttr2Usage(attribName);
		if (usage == VertexUsage::NONE)
		{
			LOG_ERROR("can't get usage for attribute '%s' in '%s'.",
				attribName.c_str(), fileName_.c_str());
			return false;
		}

		attributes_[int(usage)] = attribLocation;
	}
	return true;
}

bool ShaderProgram::parseUniforms()
{
	GLint nUniforms;
	glGetProgramiv(handle_, GL_ACTIVE_UNIFORMS, &nUniforms);

	if (nUniforms == 0)
	{
		return false;
	}

	GLint nMaxNameLen;
	glGetProgramiv(handle_, GL_ACTIVE_UNIFORM_MAX_LENGTH, &nMaxNameLen);

	std::string uniformName(nMaxNameLen + 1, '\0');
	unsigned int samplerIndex = 0;
	for (int i = 0; i < nUniforms; ++i)
	{
		uniformName.resize(nMaxNameLen);
		GLsizei len;
		GLint num;
		GLenum type;
		glGetActiveUniform(handle_, i, nMaxNameLen, &len, &num, &type, &uniformName[0]);
		uniformName.erase(len);

		GLint location = glGetUniformLocation(handle_, uniformName.c_str());

		size_t iBracket = uniformName.find('[');
		if (iBracket != std::string::npos)
		{
			uniformName.erase(iBracket);
		}

		ShaderUniform* uniform = uniformRoot_->getChildren(uniformName, true);
		uniform->pEffect_ = this;
		uniform->location_ = location;
		uniform->type_ = type;
		if (type == GL_SAMPLER_2D || type == GL_SAMPLER_CUBE)
		{
			uniform->index_ = samplerIndex++;
		}
		else
		{
			uniform->index_ = 0;
		}
	}
	return true;
}

void ShaderProgram::bind()
{
    GL_ASSERT(glUseProgram(handle_));
}

void ShaderProgram::unbind()
{
    GL_ASSERT(glUseProgram(0));
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
    //4.1及以上版本才支持 glProgramUniformMatrix4fv
	glUniformMatrix4fv(location, 1, false, data);
}

void ShaderProgram::setMatrixTranspose(int location, const float * data)
{
	glUniformMatrix4fv(location, 1, true, data);
}

ShaderUniform* ShaderProgram::findUniform(const std::string &name)
{
    return uniformRoot_->getChild(name);
}
