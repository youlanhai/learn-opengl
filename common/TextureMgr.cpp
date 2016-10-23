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
    
}

void TextureMgr::purge(const std::string &fileName)
{

}

void TextureMgr::purge(TexturePtr texture)
{

}
