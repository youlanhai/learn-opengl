#include "TextureMgr.h"
#include "LogTool.h"

IMPLEMENT_SINGLETON(TextureMgr);


TextureMgr::TextureMgr()
{
}

TextureMgr::~TextureMgr()
{
}

TexturePtr TextureMgr::get(const std::string &fileName, bool load)
{
    auto it = textures_.find(fileName);
    if(it != textures_.end())
    {
        return it->second;
    }
    
    if(load)
    {
        TexturePtr tex = new Texture();
        if(tex->load(fileName))
        {
            textures_[fileName] = tex;
            return tex;
        }
        
        LOG_ERROR("Failed to load texture: %s", fileName.c_str());
    }
    return nullptr;
}

void TextureMgr::purge(const std::string &fileName)
{
    auto it = textures_.find(fileName);
    if(it != textures_.end())
    {
        textures_.erase(it);
    }
}

void TextureMgr::purge(TexturePtr texture)
{
    for(auto it = textures_.begin(); it != textures_.end(); ++it)
    {
        if(it->second == texture)
        {
            textures_.erase(it);
            return;
        }
    }
}
