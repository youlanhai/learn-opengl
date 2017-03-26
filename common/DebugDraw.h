#pragma once
#include "Singleton.h"
#include "SmartPointer.h"
#include "Vertex.h"
#include <vector>

class Renderer;
class Mesh;
class Vector3;
class Color;
class AABB;
class Matrix;

class DebugDraw : public Singleton<DebugDraw>
{
public:
    DebugDraw();

    bool init();

    void draw(Renderer *renderer);

    void drawPoint(const Vector3 &point, const Color &color);

    void drawPoints(const Vector3 *point, int count, const Color &color);

    void drawLine(const Vector3 &start, const Vector3 &end, const Color &color);

    void drawPolygon(const Vector3 *points, int count, const Color &color);

    void drawAABB(const AABB &aabb, const Matrix &matrix, const Color &color);

    void drawFilledTriangle(const Vector3 &a, const Vector3 &b, const Vector3 &c, const Color &color);
    void drawFilledTriangle(const Vector3 &a, const Vector3 &b, const Vector3 &c, const Color &color, const Matrix &matrix, float offset = 0.01f);

private:
    SmartPointer<Mesh> mesh_;
    std::vector<VertexXYZColor> vertices_;
};
