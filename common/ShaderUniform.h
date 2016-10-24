#ifndef SHADER_UNIFORM_H
#define SHADER_UNIFORM_H

#include "Reference.h"
#include "SmartPointer.h"
#include <string>
#include <map>


class ShaderProgram;
class Texture;
class Color;
class Vector2;
class Vector3;
class Vector4;
class Matrix;

//自动常量类型
namespace AutoUniform
{
    const std::string World = "u_matWorld";
    const std::string View = "u_matView";
    const std::string Proj = "u_matProj";
    const std::string ViewProj = "u_matViewProj";
    const std::string WorldViewProj = "u_matWorldViewProj";
    const std::string AmbientColor = "u_ambient";
    const std::string OmitLight = "u_omitLight";
    const std::string DirLight = "u_dirLight";
    const std::string SpotLight = "u_spotLight";
    const std::string Texture = "u_texture";
    const std::string Texture0 = "u_texture0";
    const std::string Texture1 = "u_texture1";
    const std::string Texture2 = "u_texture2";
    const std::string Texture3 = "u_texture3";
    const std::string Texture4 = "u_texture4";
    const std::string Texture5 = "u_texture5";
    const std::string Texture6 = "u_texture6";
    const std::string Texture7 = "u_texture7";
    const std::string Material = "u_material";
    const std::string ActionAlpha = "u_actionAlpha";
    const std::string ColdDownAlpha = "u_coldDownAlpha";
    const std::string ProgressReference = "u_progressReference";
}

/**
 * Represents a uniform variable within an effect.
 */
class ShaderUniform
{
    friend class ShaderProgram;

public:

    const std::string& getName() const{ return name_; }
    const uint32_t getType() const{ return type_; }
    ShaderProgram* getProgram() const{ return pEffect_; }

    ShaderUniform *getChild(const std::string & name, bool createIfMiss = false);
    ShaderUniform *getChildren(const std::string & name, bool createIfMiss = false);

    void bindValue(float value);

    void bindValue(const float* values, int count = 1);

    void bindValue(int value);

    void bindValue(const int* values, int count = 1);

    void bindValue(const Matrix& value);

    void bindValue(const Matrix* values, int count, bool transposed);

    void bindValue(const Vector2& value);

    void bindValue(const Vector2* values, int count = 1);

    void bindValue(const Vector3& value);

    void bindValue(const Vector3* values, int count = 1);

    void bindValue(const Vector4& value);

    void bindValue(const Vector4* values, int count = 1);

    void bindValue(const Color & color);

    void bindValue(Texture* texture);

private:

    ShaderUniform(const std::string & name);
    ~ShaderUniform();

    std::string         name_;
    int                 location_;
    uint32_t            type_;
    uint32_t            index_;
    ShaderProgram*      pEffect_;
    //如果是纹理，这里需要持有它的一个引用技术，防止纹理提前析构而引起崩溃
    SmartPointer<Texture>  texture_;
    std::map<std::string, ShaderUniform*> children_;
};

class ShaderAutoUniform
{
public:

    virtual void apply(ShaderUniform *uniform) = 0;

    static ShaderAutoUniform * get(const std::string & name);
    static void set(const std::string & name, ShaderAutoUniform *autoConst);
    static void fini();

protected:
    ShaderAutoUniform();
    virtual ~ShaderAutoUniform();

    static std::map<std::string, ShaderAutoUniform*> s_autoConstMap;
};

#endif // SHADER_UNIFORM_H
