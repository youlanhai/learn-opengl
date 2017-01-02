#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H

#include "Reference.h"
#include "SmartPointer.h"
#include "VertexUsage.h"

#include <vector>

class VertexDeclaration;
class ShaderUniform;
class ShaderAutoUniform;

class ShaderProgram : public ReferenceCount
{
public:
    ShaderProgram();
    ~ShaderProgram();

    bool loadFromFile(const std::string &fileName);
    bool loadFromData(const std::string &data);

    uint32_t getHandle() const { return handle_; }
    std::string getLinkError() const;
    
    void bind();
    void unbind();
    
    int getUniformLocation(const char *name);
    int getAttribLocation(const char *name);
    int getAttribLocation(VertexUsage usage){ return attributes_[(int)usage]; }

	void setMatrix(int location, const float *data);
	void setMatrixTranspose(int location, const float *data);
    
    ShaderUniform* findUniform(const std::string &name);

    void applyAutoUniforms();

private:
	bool parseAttributes();
	bool parseUniforms();

    uint32_t        handle_;
    std::string     fileName_;
	int             attributes_[VertexUsageMax];
	ShaderUniform*	uniformRoot_;
    std::vector<std::pair<ShaderAutoUniform*, ShaderUniform*>> autoUnfiorms_;
};

typedef SmartPointer<ShaderProgram> ShaderProgramPtr;

#endif //SHADER_PROGRAM_H
