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
		const aiFace &face = mesh->mFaces[i];
		if (face.mNumIndices != 3)
			continue;

		for (size_t i = 0; i < face.mNumIndices; ++i)
		{
			*p++ = T(face.mIndices[i]);
		}
	}

	IndexBufferPtr ib = new IndexBufferEx<T>(BufferUsage::Static, p - indices, indices);
	delete[] indices;
	return ib;
}

MeshPtr processMesh(const aiMesh *mesh)
{
	//LOG_DEBUG("Num Vertices %d", mesh->mNumVertices);
	//LOG_DEBUG("Num Faces %d", mesh->mNumFaces);
	//LOG_DEBUG("Num Bones %d", mesh->mNumBones);
	//LOG_DEBUG("Num AnimMeshes %d", mesh->mNumAnimMeshes);

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

		const aiVector3D *uvs = mesh->mTextureCoords[0];
		if (uvs)
		{
			v.uv.set(uvs[i].x, uvs[i].y);
		}
		else
		{
			v.uv.set(0, 0);
		}
	}
	VertexBufferPtr vb = new VertexBufferEx<MeshVertex>(BufferUsage::Static, mesh->mNumVertices, p);
	delete[] p;
	p = nullptr;

	IndexBufferPtr ib;
	size_t nIndices = mesh->mNumFaces * 3;
	if (nIndices == 0)
	{
		ib = nullptr;
	}
	else if (nIndices < 256)
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
		n->localTransform_.transpose();

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

	const unsigned int flags =
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_GenSmoothNormals |
		aiProcess_SortByPType |
		aiProcess_OptimizeMeshes |
		aiProcess_OptimizeGraph |
		aiProcess_FlipUVs |
		aiProcess_MakeLeftHanded |
		0;

	const aiScene* scene = importer.ReadFile(fullPath, flags);
	if (scene == nullptr || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		LOG_ERROR("Failed to import model '%s', error: %s", fullPath.c_str(), importer.GetErrorString());
		return false;
	}

	//LOG_DEBUG("Num Meshes: %d", scene->mNumMeshes);

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
		MeshPtr newMesh = processMesh(mesh);
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

void Model::draw(Renderer *renderer)
{
	for (auto & info : drawInfo_)
	{
		if (info.visible)
		{
			for (int i : info.meshes)
			{
                renderer->pushMatrix();
                renderer->getWorldMatrix().preMultiply(info.node->worldTransform_);

				meshes_[i]->draw(renderer);

                renderer->popMatrix();
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
