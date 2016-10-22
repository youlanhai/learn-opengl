#ifndef COMMON_SHADER_HP
#define COMMON_SHADER_HP

#include <GLFW/glfw3.h>
#include <string>

class Shader
{
public:
    explicit Shader(GLuint type);
    ~Shader();

    bool loadFromFile(const std::string &fileName);
    bool loadFromData(const std::string &data);

    GLuint getHandle() const { return handle_; }
    std::string getCompileError() const;

    const std::string& getFileName() const { return fileName_; }
    
private:
    GLuint          handle_;
    GLuint          type_;
    std::string     fileName_;
};

#endif /* COMMON_SHADER_HP */
