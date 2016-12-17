#ifndef DEMO_TOOL_H
#define DEMO_TOOL_H

#include <string>
#include <vector>
#include "SmartPointer.h"

std::string findResPath();

class Mesh;
class Vector2;
class Vector3;
SmartPointer<Mesh> createSimpleGround(const Vector2 &size, float height,  float gridSize, float waveSize);

struct MeshVertex;
void createSimpleGround(std::vector<MeshVertex> &vertices, std::vector<uint16_t> &indices,
                        const Vector2 &size, float height,  float gridSize, float waveSize);

SmartPointer<Mesh> createPlane(const Vector2 &size, float gridSize);

SmartPointer<Mesh> createCube(const Vector3 &size);

#endif //DEMO_TOOL_H
