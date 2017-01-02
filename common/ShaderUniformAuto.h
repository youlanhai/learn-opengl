#pragma once

#include "ShaderUniform.h"

typedef void (*ShaderUniformApplyFun)(ShaderUniform *);

class ShaderAutoUniformProxy : public ShaderAutoUniform
{
    ShaderUniformApplyFun fun_;
public:
    ShaderAutoUniformProxy(ShaderUniformApplyFun fun);
    ~ShaderAutoUniformProxy();

    virtual void apply(ShaderUniform *pUniform) override;
};

void registerDefaultAutoShaderUniform();
