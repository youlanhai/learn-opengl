#ifndef DEMO_TOOL_H
#define DEMO_TOOL_H

#include <string>
#include <vector>

std::string findResPath();

struct VertexXYZNUV;
class Vector2;
void createSimpleGround(std::vector<VertexXYZNUV> &vertices,
                        std::vector<uint16_t> &indices,
                        const Vector2 &size, float height,  float gridSize, float waveSize);

#endif //DEMO_TOOL_H
