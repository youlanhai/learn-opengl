#pragma once

#include "Texture.h"

class Texture2DArray : public Texture
{
public:
    Texture2DArray();
    ~Texture2DArray();
    
    /** 暂不支持加载 */
    virtual bool load(const std::string & fileName);
    
    /** 暂不支持保存 */
    virtual bool save(const std::string & fileName) const;
    
    virtual bool create(int levels, uint32_t width, uint32_t height, TextureFormat format, int layerCount);
    
    int getLayerCount() const { return layerCount_; }
    
private:
    int     layerCount_;
};
