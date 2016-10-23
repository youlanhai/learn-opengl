#ifndef TEXTURE_MGR
#define TEXTURE_MGR

#include "Texture.h"
#include "Singleton.h"

#include <unordered_map>

class TextureMgr : public Singleton<TextureMgr>
{
public:
    TextureMgr();
    ~TextureMgr();

    TexturePtr get(const std::string &fileName, bool load = true);
    void purge(const std::string &fileName);
    void purge(TexturePtr texture);

private:
	std::unordered_map<std::string, TexturePtr> textures_;
};


#endif /* defined(TEXTURE_MGR) */
