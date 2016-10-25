#ifndef SHADER_PROGRAM_MGR_H
#define SHADER_PROGRAM_MGR_H

#include "ShaderProgram.h"
#include "Singleton.h"

#include <unordered_map>

class ShaderProgramMgr : public Singleton<ShaderProgramMgr>
{
public:
    ShaderProgramMgr();
    ~ShaderProgramMgr();

    ShaderProgramPtr get(const std::string &fileName, bool load = true);
    void purge(const std::string &fileName);
    void purge(ShaderProgramPtr shader);

private:
	std::unordered_map<std::string, ShaderProgramPtr> cache_;
};


#endif /* defined(SHADER_PROGRAM_MGR_H) */
