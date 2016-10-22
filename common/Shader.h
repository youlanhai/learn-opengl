#ifndef COMMON_SHADER_HP
#define COMMON_SHADER_HP

#include <string>

class Shader
{
public:
    explicit Shader(uint32_t type);
    ~Shader();

    bool loadFromFile(const std::string &fileName);
    bool loadFromData(const std::string &data);

	uint32_t getHandle() const { return handle_; }
    std::string getCompileError() const;

    const std::string& getFileName() const { return fileName_; }
    
private:
	uint32_t        handle_;
	uint32_t        type_;
    std::string     fileName_;
};

#endif /* COMMON_SHADER_HP */
