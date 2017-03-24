#include "DemoTool.h"
#include "PathTool.h"

#include "Vertex.h"
#include "Mesh.h"
#include "Matrix.h"

#include <unordered_map>
#include <unordered_set>
#include <map>
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

template<typename VertexType, typename IndexType>
MeshPtr createMesh(const VertexType *vertices, size_t nVertices, const IndexType *indices, size_t nIndices)
{
	VertexBufferPtr vb = new VertexBufferEx<VertexType>(BufferUsage::Static, nVertices, vertices);
	IndexBufferPtr ib = new IndexBufferEx<IndexType>(BufferUsage::Static, nIndices, indices);

	MeshPtr mesh = new Mesh();
	mesh->setVertexBuffer(vb);
	mesh->setIndexBuffer(ib);
	mesh->setVertexDecl(VertexDeclMgr::instance()->get(VertexType::getType()));

	SubMeshPtr subMesh = new SubMesh();
	subMesh->setPrimitive(PrimitiveType::TriangleList, 0, nIndices, 0, true);
	mesh->addSubMesh(subMesh);

	return mesh;
}

// abc按逆时针排列
void computeNormal(Vector3 &normal, const Vector3 &a, const Vector3 &b, const Vector3 &c)
{
    Vector3 e1 = c - a;
    Vector3 e2 = b - a;
    normal.crossProduct(e1, e2);
    normal.normalize();
}

// 是否正向。即法线是否与viewDir反向。
bool isFrontFace(const Vector3 &a, const Vector3 &b, const Vector3 &c, const Vector3& viewDir)
{
	Vector3 e1 = c - a;
	Vector3 e2 = b - a;

	Vector3 normal;
	normal.crossProduct(e1, e2);

	return normal.dotProduct(viewDir) < 0.0f;
}

// abc按逆时针排列
void computeTangent(Vector3 &tangent, const MeshVertex &a, const MeshVertex &b, const MeshVertex &c)
{
	Vector3 e1 = c.position - a.position;
	Vector3 e2 = b.position - a.position;

	Vector2 u1 = c.uv - a.uv;
	Vector2 u2 = b.uv - a.uv;

	float f = 1.0f / (u1.x * u2.y - u1.y * u2.x);

	tangent.x = f * (u2.y * e1.x - u1.y * e2.x);
	tangent.y = f * (u2.y * e1.y - u1.y * e2.y);
	tangent.z = f * (u2.y * e1.z - u1.y * e2.z);

	tangent.normalize();
}

void computeNormals(std::vector<MeshVertex> &vertices, std::vector<uint16_t> &indices)
{
	std::unordered_map<int, std::vector<int>> vertexFaces;

	// compute face normal
	std::vector<Vector3> normals;
	normals.resize(indices.size() / 3);
	for (size_t i = 0; i < indices.size(); i += 3)
	{
		int iFace = i / 3;
		computeNormal(normals[iFace],
			vertices[indices[i + 0]].position,
			vertices[indices[i + 1]].position,
			vertices[indices[i + 2]].position);

		vertexFaces[indices[i + 0]].push_back(iFace);
		vertexFaces[indices[i + 1]].push_back(iFace);
		vertexFaces[indices[i + 2]].push_back(iFace);
	}

	// slerp normal
	for (size_t i = 0; i < vertices.size(); ++i)
	{
		auto &vertex = vertices[i];
		auto &faces = vertexFaces[i];
		if (faces.empty())
		{
			vertex.normal = Vector3::YAxis;
		}
		else
		{
			vertex.normal.setZero();
			for (int iFace : faces)
			{
				vertex.normal += normals[iFace];
			}
			vertex.normal /= (float)faces.size();
		}
	}
}


void computeTangents(std::vector<MeshVertex> &vertices, std::vector<uint16_t> &indices)
{
	std::unordered_map<int, std::vector<int>> vertexFaces;

	// compute face normal
	std::vector<Vector3> tangents;
	tangents.resize(indices.size() / 3);
	for (size_t i = 0; i < indices.size(); i += 3)
	{
		int iFace = i / 3;
		computeTangent(tangents[iFace],	vertices[indices[i + 0]], vertices[indices[i + 1]],	vertices[indices[i + 2]]);

		vertexFaces[indices[i + 0]].push_back(iFace);
		vertexFaces[indices[i + 1]].push_back(iFace);
		vertexFaces[indices[i + 2]].push_back(iFace);
	}

	// slerp normal
	for (size_t i = 0; i < vertices.size(); ++i)
	{
		auto &vertex = vertices[i];
		auto &faces = vertexFaces[i];
		if (faces.empty())
		{
			vertex.tangent = Vector3::XAxis;
		}
		else
		{
			vertex.tangent.setZero();
			for (int iFace : faces)
			{
				vertex.tangent += tangents[iFace];
			}
			vertex.tangent /= (float)faces.size();
		}
	}
}

void createSimpleGround(std::vector<MeshVertex> &vertices, std::vector<uint16_t> &indices,
                        const Vector2 &size, float height,  float gridSize, float waveSize)
{
    int cols = int(size.x / gridSize) + 1;
    int rows = int(size.y / gridSize) + 1;
    
	float XLength = (cols - 1) * gridSize;
	float ZLength = (rows - 1) * gridSize;

	float halfX = XLength * 0.5f;
	float halfZ = ZLength * 0.5f;
    
    typedef MeshVertex VertexType;
    vertices.clear();
    indices.clear();
    
    // compute position and uv
    for(int r = 0; r < rows; ++r)
    {
        for(int c = 0; c < cols; ++c)
        {
            VertexType v;
            v.position.x = c * gridSize - halfX;
            v.position.z = halfZ - r * gridSize;
            
            float centerX = floor(c * gridSize / waveSize) + waveSize * 0.5f;
            float centerZ = floor(r * gridSize / waveSize) + waveSize * 0.5f;
            float dx = c * gridSize / waveSize - centerX;
            float dz = r * gridSize / waveSize - centerZ;
            
            float radius = sqrt(dx * dx + dz * dz);
            
            v.position.y = std::max(0.0, cos(radius / waveSize * PI_FULL)) * height;
            v.uv.x = (c * gridSize) / XLength;
            v.uv.y = (r * gridSize) / ZLength;
            
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
    
	computeNormals(vertices, indices);
	computeTangents(vertices, indices);
}

MeshPtr createSimpleGround(const Vector2 &size, float height, float gridSize, float waveSize)
{
	typedef MeshVertex VertexType;
	std::vector<VertexType> vertices;
	std::vector<uint16_t> indices;

	createSimpleGround(vertices, indices, size, height, gridSize, waveSize);
	return createMesh<VertexType, uint16_t>(vertices.data(), vertices.size(), indices.data(), indices.size());
}

void createPlane(std::vector<MeshVertex> &vertices, std::vector<uint16_t> &indices,
	const Vector2 &size, float gridSize)
{
	int cols = (int)ceil(size.x / gridSize) + 1;
	int rows = (int)ceil(size.y / gridSize) + 1;

	float XLength = (cols - 1) * gridSize;
	float ZLength = (rows - 1) * gridSize;

	float halfX = XLength * 0.5f;
	float halfZ = ZLength * 0.5f;

	typedef MeshVertex VertexType;
	vertices.clear();
	indices.clear();

	// compute position and uv
	for (int r = 0; r < rows; ++r)
	{
		for (int c = 0; c < cols; ++c)
		{
			VertexType v;
			v.position.x = c * gridSize - halfX;
			v.position.y = 0.0f;
			v.position.z = halfZ - r * gridSize;

			v.normal = Vector3::YAxis;
			v.tangent = Vector3::XAxis;

			v.uv.x = (c * gridSize) / XLength;
			v.uv.y = (r * gridSize) / ZLength;

			vertices.push_back(v);
		}
	}

	// create indices
	for (int r = 0; r < rows - 1; ++r)
	{
		for (int c = 0; c < cols - 1; ++c)
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
}

MeshPtr createPlane(const Vector2 &size, float gridSize)
{
	typedef MeshVertex VertexType;
	std::vector<VertexType> vertices;
	std::vector<uint16_t> indices;

	createPlane(vertices, indices, size, gridSize);
	return createMesh<VertexType, uint16_t>(vertices.data(), vertices.size(), indices.data(), indices.size());
}

void createCube(std::vector<MeshVertex> &vertices, std::vector<uint16_t> &indices,
	const Vector3 &size)
{
	float X = size.x * 0.5f;
	float Y = size.y * 0.5f;
	float Z = size.z * 0.5f;

	float buffer[] = {
		//position normal uv tangent
		// front
		-X, Y, -Z,	0, 0, -1, 0, 0,
		-X, -Y, -Z,	0, 0, -1, 0, 1,
		X, Y, -Z,	0, 0, -1, 1, 0,
		X, -Y, -Z,	0, 0, -1, 1, 1,
		// back
		X, Y, Z,	0, 0, 1, 0, 0,
		X, -Y, Z,	0, 0, 1, 0, 1,
		-X, Y, Z,	0, 0, 1, 1, 0,
		-X, -Y, Z,	0, 0, 1, 1, 1,
		// left
		-X, Y, Z,	-1, 0, 0, 0, 0,
		-X, -Y, Z,	-1, 0, 0, 0, 1,
		-X, Y, -Z,	-1, 0, 0, 1, 0,
		-X, -Y, -Z,	-1, 0, 0, 1, 1,
		// right
		X, Y, -Z,	1, 0, 0, 0, 0,
		X, -Y, -Z,	1, 0, 0, 0, 1,
		X, Y, Z,	1, 0, 0, 1, 0,
		X, -Y, Z,	1, 0, 0, 1, 1,
		// top 
		-X, Y, Z,	0, 1, 0, 0, 0,
		-X, Y, -Z,	0, 1, 0, 0, 1,
		X, Y, Z,	0, 1, 0, 1, 0,
		X, Y, -Z,	0, 1, 0, 1, 1,
		// bottom
		-X, -Y, -Z,	0, -1, 0, 0, 0,
		-X, -Y, Z,	0, -1, 0, 0, 1,
		X, -Y, -Z,	0, -1, 0, 1, 0,
		X, -Y, Z,	0, -1, 0, 1, 1,
	};

	// copy vertices data
	vertices.resize(6 * 4);
	for (int n = 0; n < 6 * 4; ++n)
	{
		MeshVertex &v = vertices[n];

		int i = n * 8;
		v.position.set(buffer[i], buffer[i + 1], buffer[i + 2]);
		
        v.normal.set(buffer[i + 3], buffer[i + 4], buffer[i + 5]);
        //v.normal = v.position;
        //v.normal.normalize();

		v.uv.set(buffer[i + 6], buffer[i + 7]);
		v.tangent.setZero();
	}

	// compute tangent
	for (int i = 0; i < 6 * 4; i += 4)
	{
		Vector3 tangent;
		computeTangent(tangent, vertices[i], vertices[i + 1], vertices[i + 2]);

		for (int j = 0; j < 4; ++j)
		{
			vertices[i + j].tangent = tangent;
		}
	}

	// generate indices
	indices.clear();
	indices.reserve(6 * 6);
	for (int n = 0; n < 6; ++n)
	{
		int i = n * 4;

		indices.push_back(i + 0);
		indices.push_back(i + 1);
		indices.push_back(i + 2);

		indices.push_back(i + 2);
		indices.push_back(i + 1);
		indices.push_back(i + 3);
	}
}

MeshPtr createCube(const Vector3 &size)
{
	typedef MeshVertex VertexType;
	std::vector<VertexType> vertices;
	std::vector<uint16_t> indices;

	createCube(vertices, indices, size);
	return createMesh<VertexType, uint16_t>(vertices.data(), vertices.size(), indices.data(), indices.size());
}

MeshPtr createQuad(const Vector2 & size)
{
	typedef VertexXYZUV VertexType;

	float hx = size.x * 0.5f;
	float hy = size.y * 0.5f;

	VertexType vertices[4] = {
        { { -hx, hy, 0.0f},     { 0.0f, 0.0f}, }, // left top
        { { -hx, -hy, 0.0f },   { 0.0f, 1.0f} }, // left bottom
        { { hx, hy, 0.0f },     { 1.0f, 0.0f}}, // right top
        { { hx, -hy, 0.0f },    { 1.0f, 1.0f}}, // right bottom
	};

	uint16_t indices[6] = {
		0, 1, 2,
		2, 1, 3,
	};

	return createMesh<VertexType, uint16_t>(vertices, 4, indices, 6);
}

static uint32_t extractIndex(const char *data, size_t i, size_t stride)
{
	if (stride == 1)
	{
		return ((uint8_t*)data)[i];
	}
	else if (stride == 2)
	{
		return ((uint16_t*)data)[i];
	}
	else if (stride == 4)
	{
		return ((uint32_t*)data)[i];
	}
	assert(0 && "shouldn't reach here!");
	return 0;
}

typedef std::unordered_map<uint64_t, uint64_t> EdgeSet;
void unionEdge(EdgeSet &edges, uint32_t a, uint32_t b)
{
	uint64_t e;
	if (a <= b)
	{
		e = (uint64_t)a << 32 | (uint64_t)b;
	}
	else
	{
		e = (uint64_t)b << 32 | (uint64_t)a;
	}

	auto it = edges.find(e);
	if (it == edges.end())
	{
		edges[e] = (uint64_t)a << 32 | (uint64_t)b;
	}
	else
	{
		// 如果已经存在该边，说明这不是边界
		edges.erase(it);
	}
}


MeshPtr createShaowVolumeForDirectionLight(MeshPtr source, const Matrix &matWorld, const Vector3 &lightDir)
{
	VertexDeclarationPtr decl = source->getVertexDecl();
	VertexBufferPtr vb = source->getVertexBuffer();
	IndexBufferPtr ib = source->getIndexBuffer();

	if (decl->getName() != MeshVertex::getType())
	{
		return nullptr;
	}

	EdgeSet edges;

	std::map<uint32_t, uint32_t> frontIndexSet;
	std::vector<uint32_t> frontIndices;

	const MeshVertex *pVertex = (const MeshVertex*)vb->lock(true);
	const char *pIndex = ib->lock(true);
	for (size_t i = 0; i < ib->count(); i += 3)
	{
		uint32_t i0 = extractIndex(pIndex, i + 0, ib->stride());
		uint32_t i1 = extractIndex(pIndex, i + 1, ib->stride());
		uint32_t i2 = extractIndex(pIndex, i + 2, ib->stride());

		Vector3 a = matWorld.transformPoint(pVertex[i0].position);
		Vector3 b = matWorld.transformPoint(pVertex[i1].position);
		Vector3 c = matWorld.transformPoint(pVertex[i2].position);

		if (isFrontFace(a, b, c, lightDir))
		{
			frontIndices.push_back(i0);
			frontIndices.push_back(i1);
			frontIndices.push_back(i2);

			frontIndexSet[i0] = 0;
			frontIndexSet[i1] = 0;
			frontIndexSet[i2] = 0;

			unionEdge(edges, i0, i1);
			unionEdge(edges, i1, i2);
			unionEdge(edges, i2, i0);
		}
	}
	ib->unlock();
	vb->unlock();

	typedef VertexXYZ VertexType;
	std::vector<VertexType> vertices;
	std::vector<uint16_t> indices;

	// 拷贝正面的顶点数据
	vertices.reserve(frontIndexSet.size());
	VertexType vertex;
	for (auto & pair : frontIndexSet)
	{
		// 将正面的索引重新映射为更紧凑的索引
		pair.second = vertices.size();

		vertex.position = matWorld.transformPoint(pVertex[pair.first].position);
		vertex.position += lightDir * 0.1f;
		vertices.push_back(vertex);
	}

	// 拷贝正面的索引数据
	indices.reserve(frontIndices.size());
	for (uint32_t index : frontIndices)
	{
		auto it = frontIndexSet.find(index);
		assert(it != frontIndexSet.end());
		indices.push_back(it->second);
	}

	// 向无穷远处平移，生成背面顶点数据
	size_t nVertices = vertices.size();
	vertices.reserve(nVertices * 2.0f);
	for (size_t i = 0; i < nVertices; ++i)
	{
		vertices.push_back(vertices[i]);
		vertices.back().position += lightDir * 10.0f;
	}
	size_t nIndices = indices.size();
	for (size_t i = 0; i < nIndices; i += 3)
	{
		// 背面的顶点顺序跟正面相反
		indices.push_back(indices[i + 0] + nVertices);
		indices.push_back(indices[i + 2] + nVertices);
		indices.push_back(indices[i + 1] + nVertices);
	}

	// 生成侧面
	for (auto &pair : edges)
	{
		uint64_t e = pair.second;
		uint32_t a = (e >> 32) & 0xffffffff;
		uint32_t b = e & 0xffffffff;

		a = frontIndexSet[a];
		b = frontIndexSet[b];
		uint32_t c = a + nVertices;
		uint32_t d = b + nVertices;

		indices.push_back(a);
		indices.push_back(c);
		indices.push_back(b);

		indices.push_back(b);
		indices.push_back(c);
		indices.push_back(d);
	}

	MeshPtr ret = new Mesh();
	ret->setVertexBuffer(new VertexBufferEx<VertexType>(BufferUsage::Static, vertices.size(), vertices.data()));
	ret->setIndexBuffer(new IndexBufferEx<uint16_t>(BufferUsage::Static, indices.size(), indices.data()));
	ret->setVertexDecl(VertexDeclMgr::instance()->get(VertexType::getType()));

	SubMeshPtr subMesh = new SubMesh();
	subMesh->setPrimitive(PrimitiveType::TriangleList, 0, indices.size(), 0, true);
	ret->addSubMesh(subMesh);
	return ret;
}
