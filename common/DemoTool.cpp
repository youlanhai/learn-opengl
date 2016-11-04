#include "DemoTool.h"
#include "PathTool.h"

#include "Vertex.h"
#include "Mesh.h"

#include <cmath>

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

#include <algorithm>

std::string findResPath()
{
    std::string rootPath = getExePath();
    std::string resPath;
    while(!rootPath.empty() && !isDir(resPath = joinPath(rootPath, "res")))
    {
        rootPath = getFilePath(rootPath);
    }
    return resPath;
}

void computeNormal(Vector3 &normal, const Vector3 &a, const Vector3 &b, const Vector3 &c)
{
    Vector3 e1 = b - a;
    Vector3 e2 = c - a;
    normal.crossProduct(e1, e2);
    normal.normalize();
}

SmartPointer<Mesh> createSimpleGround(const Vector2 &size, float height,  float gridSize, float waveSize)
{
    typedef VertexXYZNUV VertexType;
    std::vector<VertexType> vertices;
    std::vector<uint16_t> indices;
    
    createSimpleGround(vertices, indices, size, height, gridSize, waveSize);
    
    VertexBufferPtr vb = new VertexBufferEx<VertexType>(BufferUsage::Static, vertices.size(), vertices.data());
    IndexBufferPtr ib = new IndexBufferEx<uint16_t>(BufferUsage::Static, indices.size(), indices.data());
    
    MeshPtr mesh = new Mesh();
    mesh->setVertexBuffer(vb);
    mesh->setIndexBuffer(ib);
    mesh->setVertexDecl(VertexDeclMgr::instance()->get(VertexType::getType()));
    
    SubMeshPtr subMesh = new SubMesh();
    subMesh->setPrimitive(PrimitiveType::TriangleList, 0, indices.size(), 0, true);
    mesh->addSubMesh(subMesh);
    return mesh;
}

void createSimpleGround(std::vector<VertexXYZNUV> &vertices, std::vector<uint16_t> &indices,
                        const Vector2 &size, float height,  float gridSize, float waveSize)
{
    int cols = int(size.x / gridSize) + 1;
    int rows = int(size.y / gridSize) + 1;
    
    float halfX = (cols - 1) * gridSize * 0.5f;
    float halfZ = (rows - 1) * gridSize * 0.5f;
    
    float pi = 3.141592654f;
    
    typedef VertexXYZNUV VertexType;
    vertices.clear();
    indices.clear();
    
    // compute position and uv
    for(int r = 0; r < rows; ++r)
    {
        for(int c = 0; c < cols; ++c)
        {
            VertexType v;
            v.position.x = c * gridSize - halfX;
            v.position.z = r * gridSize - halfZ;
            
            float hx = sin(v.position.x / waveSize * pi * 2);
            float hz = sin(v.position.z / waveSize * pi * 2);
            
            v.position.y = std::min(hx, hz) * height;
            v.uv.x = (c * gridSize) / (halfX * 2.0f);
            v.uv.y = (r * gridSize) / (halfZ * 2.0f);
            
            vertices.push_back(v);
        }
    }
    
    // create indices
    for(int r = 0; r < rows - 1; ++r)
    {
        for(int c = 0; c < cols - 1; ++c)
        {
            int i = r * cols + c;
            indices.push_back(i); //left top
            indices.push_back(i + cols); // left bottom
            indices.push_back(i + 1); // right top
            
            
            indices.push_back(i + 1); // right top
            indices.push_back(i + cols); // left bottom
            indices.push_back(i + cols + 1); // left bottom
        }
    }
    
    // compute face normal
    std::vector<Vector3> normals;
    uint16_t *p = indices.data();
    for(int r = 0; r < rows - 1; ++r)
    {
        for(int c = 0; c < cols - 1; ++c)
        {
            Vector3 normal;
            computeNormal(normal,
                          vertices[*(p + 0)].position,
                          vertices[*(p + 1)].position,
                          vertices[*(p + 2)].position);
            normals.push_back(normal);
            
            computeNormal(normal,
                          vertices[*(p + 3)].position,
                          vertices[*(p + 4)].position,
                          vertices[*(p + 5)].position);
            normals.push_back(normal);
            p += 6;
        }
    }
    
    // slerp normal
    for(int r = 0; r < rows; ++r)
    {
        for(int c = 0; c < cols; ++c)
        {
            int face = (r * (cols - 1) + c) * 2;
            int faceUp = face - (cols - 1) * 2;
            
            // clockwise
            int faces[6] = {
                face,
                face - 1,
                face - 2,
                faceUp - 1,
                faceUp,
                faceUp + 1,
            };
            
            int n = 0;
            Vector3 normal;
            for(int i = 0; i < 6; ++i)
            {
                int f = faces[i];
                if(f >= 0 && f < normals.size())
                {
                    normal += normals[f];
                    ++n;
                }
            }
            normal /= n;
            normal.normalize();
            
            vertices[r * cols + c].normal = normal;
        }
    }
   
}
