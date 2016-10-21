#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H

#include "Reference.h"
#include <GLFW/glfw3.h>
#include <string>

class ShaderProgram : public ReferenceCount
{
public:
    ShaderProgram();
    ~ShaderProgram();

    bool loadFromFile(const std::string &fileName);
    bool loadFromData(const std::string &data);

    GLuint getHandle() const { return handle_; }
    std::string getLinkError() const;
    
    void use();
    
    int getUniformLocation(const char *name);
    int getAttribLocation(const char *name);

private:
    GLuint          handle_;
    std::string     fileName_;
};

#endif //SHADER_PROGRAM_H
