#ifndef DEMO_TOOL_H
#define DEMO_TOOL_H

#include <string>
#include <vector>
#include "SmartPointer.h"

std::string findResPath();

class Mesh;
class Vector2;
SmartPointer<Mesh> createSimpleGround(const Vector2 &size, float height,  float gridSize, float waveSize);

struct VertexXYZNUV;
void createSimpleGround(std::vector<VertexXYZNUV> &vertices, std::vector<uint16_t> &indices,
                        const Vector2 &size, float height,  float gridSize, float waveSize);

#endif //DEMO_TOOL_H
