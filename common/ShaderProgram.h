#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H

#include "VertexDeclaration.h"

class ShaderUniform;

class ShaderProgram : public ReferenceCount
{
public:
    ShaderProgram();
    ~ShaderProgram();

    bool loadFromFile(const std::string &fileName);
    bool loadFromData(const std::string &data);

    uint32_t getHandle() const { return handle_; }
    std::string getLinkError() const;
    
    void use();
    
    int getUniformLocation(const char *name);
    int getAttribLocation(const char *name);

	void setMatrix(int location, const float *data);
	void setMatrixTranspose(int location, const float *data);

private:
	bool parseAttributes();
	bool parseUniforms();

    uint32_t        handle_;
    std::string     fileName_;
	int             attributes_[VertexUsageMax];
	ShaderUniform*	uniformRoot_;
};

#endif //SHADER_PROGRAM_H
