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

SmartPointer<Mesh> createSimpleGround(const Vector2 &size, float height,  float gridSize, float waveSize);

SmartPointer<Mesh> createPlane(const Vector2 &size, float gridSize);

SmartPointer<Mesh> createCube(const Vector3 &size);

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
