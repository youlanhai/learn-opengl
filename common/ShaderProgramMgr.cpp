#include "ShaderProgramMgr.h"
#include "LogTool.h"

IMPLEMENT_SINGLETON(ShaderProgramMgr);


ShaderProgramMgr::ShaderProgramMgr()
{
}

ShaderProgramMgr::~ShaderProgramMgr()
{
}

ShaderProgramPtr ShaderProgramMgr::get(const std::string &fileName, bool load)
{
    auto it = cache_.find(fileName);
    if(it != cache_.end())
    {
        return it->second;
    }
    
    if(load)
    {
        ShaderProgramPtr res = new ShaderProgram();
        if(res->loadFromFile(fileName))
        {
            cache_[fileName] = res;
            return res;
        }
        
        LOG_ERROR("Failed to load shader program: %s", fileName.c_str());
    }
    return nullptr;
}

void ShaderProgramMgr::purge(const std::string &fileName)
{
    auto it = cache_.find(fileName);
    if(it != cache_.end())
    {
        cache_.erase(it);
    }
}

void ShaderProgramMgr::purge(ShaderProgramPtr shader)
{
    for(auto it = cache_.begin(); it != cache_.end(); ++it)
    {
        if(it->second == shader)
        {
            cache_.erase(it);
            return;
        }
    }
}
