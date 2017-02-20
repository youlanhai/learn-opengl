#ifndef DEMO_TOOL_H
#define DEMO_TOOL_H

#include <string>
#include <vector>
#include "SmartPointer.h"
#include "ShaderProgram.h"
#include "ShaderUniform.h"

std::string findResPath();

class Mesh;
class Vector2;
class Vector3;

typedef SmartPointer<Mesh> MeshPtr;

MeshPtr createSimpleGround(const Vector2 &size, float height,  float gridSize, float waveSize);

MeshPtr createPlane(const Vector2 &size, float gridSize);

MeshPtr createCube(const Vector3 &size);

MeshPtr createQuad(const Vector2 &size);

MeshPtr createShaowVolumeForDirectionLight(MeshPtr source, const Matrix &matWorld, const Vector3 &lightDir);

template<typename T>
bool bindShaderUniform(ShaderProgram *shader, const char *name, const T &value)
{
	ShaderUniform *un = shader->findUniform(name);
	if (un)
	{
		un->bindValue(value);
		return true;
	}
	return false;
}

template<typename T>
bool bindShaderUniform(ShaderProgram *shader, const char *name, const T* value, int n)
{
	ShaderUniform *un = shader->findUniform(name);
	if (un)
	{
		un->bindValue(value, n);
		return true;
	}
	return false;
}

#endif //DEMO_TOOL_H
