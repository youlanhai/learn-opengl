#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H

#include "Reference.h"
#include <string>

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
    uint32_t        handle_;
    std::string     fileName_;
};

#endif //SHADER_PROGRAM_H
