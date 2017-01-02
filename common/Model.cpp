#include "Model.h"
#include "Mesh.h"
#include "LogTool.h"
#include "FileSystem.h"
#include "PathTool.h"
#include "Vertex.h"
#include "VertexBuffer.h"
#include "VertexAttribute.h"
#include "VertexDeclaration.h"
#include "Material.h"
#include "TextureMgr.h"
#include "Renderer.h"

#include <sstream>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

template<typename T>
IndexBufferPtr extractIndices(const aiMesh *mesh)
{
	size_t nIndices = mesh->mNumFaces * 3;
	T *indices = new T[nIndices];
	T *p = indices;

	for (size_t i = 0; i < mesh->mNumFaces; ++i)
	{
		const aiFace &face = mesh->mFaces[i++];
		assert(face.mNumIndices == 3);

		for (size_t i = 0; i < face.mNumIndices; ++i)
		{
			*p++ = T(face.mIndices[i]);
		}
		LOG_DEBUG("%d %d %d", (int)face.mIndices[0], (int)face.mIndices[1], (int)face.mIndices[2]);
		//LOG_DEBUG("%d %d %d", (int)face.mIndices[3], (int)face.mIndices[4], (int)face.mIndices[5]);
	}

	return new IndexBufferEx<T>(BufferUsage::Static, nIndices, indices);
}

MeshPtr processMesh(const aiMesh *mesh)
{
	MeshVertex *p = new MeshVertex[mesh->mNumVertices];
	for (size_t i = 0; i < mesh->mNumVertices; ++i)
	{
		MeshVertex &v = p[i];
		v.position.set(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
		if (mesh->mNormals)
		{
			v.normal.set(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
		}

		if (mesh->mTangents)
		{
			v.tangent.set(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
		}

		const aiVector3D *uv = mesh->mTextureCoords[0];
		if (uv)
		{
			v.uv.set(uv->x, uv->y);
		}
		else
		{
			v.uv.set(0, 0);
		}
	}
	VertexBufferPtr vb = new VertexBufferEx<MeshVertex>(BufferUsage::Static, mesh->mNumVertices, p);

	IndexBufferPtr ib;
	size_t nIndices = mesh->mNumFaces * 3;
	if (nIndices == 0)
	{

	}
	if (nIndices < 256)
	{
		ib = extractIndices<uint8_t>(mesh);
	}
	else if (nIndices < 65536)
	{
		ib = extractIndices<uint16_t>(mesh);
	}
	else
	{
		ib = extractIndices<uint32_t>(mesh);
	}

	MeshPtr newMesh = new Mesh();
	newMesh->setVertexBuffer(vb);
	newMesh->setIndexBuffer(ib);
	newMesh->setVertexDecl(VertexDeclMgr::instance()->get(MeshVertex::getType()));

	SubMeshPtr subMesh = new SubMesh();
	if (ib)
	{
		subMesh->setPrimitive(PrimitiveType::TriangleList, 0, ib->count(), 0, true);
	}
	else
	{
		subMesh->setPrimitive(PrimitiveType::TriangleList, 0, vb->count(), 0, false);
	}
	newMesh->addSubMesh(subMesh);
	return newMesh;
}

TexturePtr processTexture(aiMaterial *mat, aiTextureType type, const std::string &resourcePath)
{
	aiString path;
	if (mat->GetTextureCount(type) > 0 && AI_SUCCESS == mat->GetTexture(type, 0, &path))
	{
		std::string texturePath = joinPath(resourcePath, path.C_Str());
		return TextureMgr::instance()->get(texturePath);
	}
	return nullptr;
}

class ModelNodeLoader
{
	Model* model_;
	const aiScene *scene_;
public:

	ModelNodeLoader(Model *model, const aiScene *scene)
		: model_(model)
		, scene_(scene)
	{
	}

	ModelNodePtr processNode(const aiNode *node)
	{
		ModelNodePtr n = new ModelNode();
		n->name_ = node->mName.C_Str();

		memcpy(n->localTransform_._m, &(node->mTransformation.a1), 16 * sizeof(float));
		//n->localTransform_.transpose();

		for (size_t i = 0; i < node->mNumMeshes; ++i)
		{
			Model::NodeDrawInfo info;
			info.node = n;
			info.visible = true;

			info.meshes.push_back(node->mMeshes[i]);
			model_->drawInfo_.push_back(info);
		}

		for (size_t i = 0; i < node->mNumChildren; ++i)
		{
			n->children_.push_back(processNode(node->mChildren[i]));
		}
		return n;
	}
};

ModelNode::ModelNode()
{
}

void ModelNode::applyMatrix(const Matrix & parentTransform)
{
	worldTransform_.multiply(parentTransform, localTransform_);

	for (auto child : children_)
	{
		child->applyMatrix(worldTransform_);
	}
}

Model::Model()
{
}

Model::~Model()
{
}

bool Model::load(const std::string & path, ShaderProgramPtr shader)
{
	resource_ = path;
	std::string resourcePath = getFilePath(path);

	std::string fullPath = FileSystem::instance()->getFullPath(path);
	if (fullPath.empty())
	{
		LOG_ERROR("Faild to find file '%s'", path.c_str());
		return false;
	}

	Assimp::Importer importer;

	// default pp steps
	unsigned int ppsteps = aiProcess_CalcTangentSpace | // calculate tangents and bitangents if possible
		aiProcess_JoinIdenticalVertices | // join identical vertices/ optimize indexing
		aiProcess_ValidateDataStructure | // perform a full validation of the loader's output
		aiProcess_ImproveCacheLocality | // improve the cache locality of the output vertices
		aiProcess_RemoveRedundantMaterials | // remove redundant materials
		aiProcess_FindDegenerates | // remove degenerated polygons from the import
		aiProcess_FindInvalidData | // detect invalid model data, such as invalid normal vectors
		aiProcess_GenUVCoords | // convert spherical, cylindrical, box and planar mapping to proper UVs
		aiProcess_TransformUVCoords | // preprocess UV transformations (scaling, translation ...)
		aiProcess_FindInstances | // search for instanced meshes and remove them by references to one master
		aiProcess_LimitBoneWeights | // limit bone weights to 4 per vertex
		aiProcess_OptimizeMeshes | // join small meshes, if possible;
		aiProcess_SplitByBoneCount | // split meshes with too many bones. Necessary for our (limited) hardware skinning shader
		0;

	const aiScene* scene = importer.ReadFile(fullPath,
		//aiProcessPreset_TargetRealtime_MaxQuality);
		//aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);
		ppsteps |
		aiProcess_GenSmoothNormals | // generate smooth normal vectors if not existing
		aiProcess_SplitLargeMeshes | // split large, unrenderable meshes into submeshes
		aiProcess_Triangulate | // triangulate polygons with more than 3 edges
		aiProcess_ConvertToLeftHanded | // convert everything to D3D left handed space
		aiProcess_SortByPType | // make 'clean' meshes which consist of a single typ of primitives
		0);
	if (scene == nullptr || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		LOG_ERROR("Failed to import model '%s', error: %s", fullPath.c_str(), importer.GetErrorString());
		return false;
	}

	Mesh::Materials mtls;
	mtls.reserve(scene->mNumMaterials);
	for (size_t i = 0; i < scene->mNumMaterials; ++i)
	{
		aiMaterial *mat = scene->mMaterials[i];

		MaterialPtr mtl = new Material();
		mtl->setShader(shader);

		TexturePtr tex;
		tex = processTexture(mat, aiTextureType_DIFFUSE, resourcePath);
		if (tex)
		{
			mtl->setTexture("u_texture0", tex);
		}
		tex = processTexture(mat, aiTextureType_NORMALS, resourcePath);
		if (tex)
		{
			mtl->setTexture("u_texture1", tex);
		}
		tex = processTexture(mat, aiTextureType_SPECULAR, resourcePath);
		if (tex)
		{
			mtl->setTexture("u_texture2", tex);
		}
		
		mtls.push_back(mtl);
	}

	for (size_t i = 0; i < scene->mNumMeshes; ++i)
	{
		aiMesh *mesh = scene->mMeshes[i];
		MeshPtr newMesh = processMesh(scene->mMeshes[i]);
		if (newMesh)
		{
			if (mesh->mMaterialIndex >= 0)
			{
				newMesh->addMaterial(mtls[mesh->mMaterialIndex]);
			}
		}
		meshes_.push_back(newMesh);
	}

	ModelNodeLoader nodeLoader(this, scene);
	if (scene->mRootNode != nullptr)
	{
		root_ = nodeLoader.processNode(scene->mRootNode);
	}

	applyMatrix(Matrix::Identity);
	return true;
}

void Model::applyMatrix(const Matrix & worldMatrix)
{
	if (root_)
	{
		root_->applyMatrix(worldMatrix);
	}
}

void Model::draw()
{
	for (auto & info : drawInfo_)
	{
		if (info.visible)
		{
			for (int i : info.meshes)
			{
				//Renderer::instance()->setMatrix(info.node->worldTransform_);
				meshes_[i]->draw();
			}
		}
	}
}

ModelNodePtr Model::findNode(const std::string & name) const
{
	auto it = nodeMap_.find(name);
	if (it != nodeMap_.end())
	{
		return it->second;
	}

	return ModelNodePtr();
}

void Model::setNodeVisible(const std::string & name, bool visible)
{
	for (auto & info : drawInfo_)
	{
		if (info.node->name_ == name)
		{
			info.visible = visible;
			return;
		}
	}
}
