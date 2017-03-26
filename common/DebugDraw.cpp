#include "DebugDraw.h"
#include "Mesh.h"
#include "VertexBuffer.h"
#include "VertexDeclaration.h"
#include "Renderer.h"

IMPLEMENT_SINGLETON(DebugDraw);

DebugDraw::DebugDraw()
{
}

bool DebugDraw::init()
{
    mesh_ = new Mesh();

    mesh_->setVertexDecl(VertexDeclMgr::instance()->get(VertexXYZColor::getType()));
    mesh_->setVertexBuffer(new VertexBufferEx<VertexXYZColor>(BufferUsage::Dynamic, 0));
    return true;
}

void DebugDraw::draw(Renderer * renderer)
{
    if (mesh_ && !vertices_.empty())
    {
        renderer->applyCameraMatrix();
        renderer->setWorldMatrix(Matrix::Identity);

        if (mesh_->getMaterials().empty())
        {
            MaterialPtr mtl = new Material();
            mtl->loadShader("shader/xyzcolor.shader");
            mesh_->addMaterial(mtl);
        }
        GLint cullFace;
        glGetIntegerv(GL_CULL_FACE, &cullFace);
        glDisable(GL_CULL_FACE);
        glDepthFunc(GL_LEQUAL);
        VertexBufferPtr vb = mesh_->getVertexBuffer();
        vb->resize(vertices_.size(), vertices_.data());

        mesh_->draw(renderer);

        mesh_->clearSubMeshes();
        glDepthFunc(GL_LESS);
        if (cullFace)
        {
            glEnable(GL_CULL_FACE);
        }
    }
    vertices_.clear();
}

void DebugDraw::drawPoint(const Vector3 & point, const Color & color)
{
    SubMeshPtr sub = new SubMesh();
    sub->setPrimitive(PrimitiveType::PointList, vertices_.size(), 1, 0, false);
    mesh_->addSubMesh(sub);

    VertexXYZColor v = { point, color };
    vertices_.push_back(v);
}

void DebugDraw::drawPoints(const Vector3 * points, int count, const Color & color)
{
    SubMeshPtr sub = new SubMesh();
    sub->setPrimitive(PrimitiveType::PointList, vertices_.size(), count, 0, false);
    mesh_->addSubMesh(sub);

    VertexXYZColor v;
    v.color = color;

    for (int i = 0; i < count; ++i)
    {
        v.position = points[i];
        vertices_.push_back(v);
    }
}

void DebugDraw::drawLine(const Vector3 & start, const Vector3 & end, const Color & color)
{
    SubMeshPtr sub = new SubMesh();
    sub->setPrimitive(PrimitiveType::LineList, vertices_.size(), 2, 0, false);
    mesh_->addSubMesh(sub);

    VertexXYZColor v;
    v.color = color;

    v.position = start;
    vertices_.push_back(v);

    v.position = end;
    vertices_.push_back(v);
}

void DebugDraw::drawPolygon(const Vector3 * points, int count, const Color & color)
{
    SubMeshPtr sub = new SubMesh();
    sub->setPrimitive(PrimitiveType::LineStrip, vertices_.size(), count + 1, 0, false);
    mesh_->addSubMesh(sub);

    VertexXYZColor v;
    v.color = color;

    for (int i = 0; i < count; ++i)
    {
        v.position = points[i];
        vertices_.push_back(v);
    }

    v.position = points[0];
    vertices_.push_back(v);
}

void DebugDraw::drawAABB(const AABB & ab, const Matrix & matrix, const Color & color)
{
    Vector3 points[8] = {
        { ab.min_.x, ab.min_.y, ab.min_.z }, // front left bottom
        { ab.min_.x, ab.max_.y, ab.min_.z }, // front left top
        { ab.max_.x, ab.max_.y, ab.min_.z }, // front right top
        { ab.max_.x, ab.min_.y, ab.min_.z }, // front right bottom

        { ab.min_.x, ab.min_.y, ab.max_.z }, // back left bottom
        { ab.min_.x, ab.max_.y, ab.max_.z }, // back left top
        { ab.max_.x, ab.max_.y, ab.max_.z }, // back right top
        { ab.max_.x, ab.min_.y, ab.max_.z }, // back right bottom
    };

    for (Vector3 &p : points)
    {
        p = matrix.transformPoint(p);
    }

    drawPolygon(points, 4, color);
    drawPolygon(points + 4, 4, color);
    for (int i = 0; i < 4; ++i)
    {
        drawLine(points[i], points[i + 4], color);
    }
}

void DebugDraw::drawFilledTriangle(const Vector3 & a, const Vector3 & b, const Vector3 & c, const Color & color)
{
    SubMeshPtr sub = new SubMesh();
    sub->setPrimitive(PrimitiveType::TriangleList, vertices_.size(), 3, 0, false);
    mesh_->addSubMesh(sub);

    VertexXYZColor v;
    v.color = color;

    v.position = a;
    vertices_.push_back(v);

    v.position = b;
    vertices_.push_back(v);

    v.position = c;
    vertices_.push_back(v);
}

void DebugDraw::drawFilledTriangle(const Vector3 & a, const Vector3 & b, const Vector3 & c, const Color & color, const Matrix & matrix, float offset)
{
    Vector3 na = matrix.transformPoint(a);
    Vector3 nb = matrix.transformPoint(b);
    Vector3 nc = matrix.transformPoint(c);

    Vector3 normal = (nc - na).crossProduct(nb - na);
    normal.normalize();
    normal *= offset;

    na += normal;
    nb += normal;
    nc += normal;

    drawFilledTriangle(na, nb, nc, color);
}
