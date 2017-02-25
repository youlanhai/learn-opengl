#pragma once

#include "Reference.h"
#include "SmartPointer.h"
#include "Matrix.h"
#include "Quaternion.h"
#include "Component.h"

#include <vector>
#include <string>
#include <unordered_map>

class ShaderProgram;
typedef SmartPointer<ShaderProgram> ShaderProgramPtr;

class Mesh;
typedef SmartPointer<Mesh> MeshPtr;

class ModelNode;
typedef SmartPointer<ModelNode> ModelNodePtr;

// 模型的骨骼结点
class ModelNode : public ReferenceCount
{
public:
	ModelNode();

	void applyMatrix(const Matrix &parentTransform);

	std::string name_;
	Matrix	worldTransform_;
	Matrix	localTransform_;

	std::vector<ModelNodePtr> children_;
};

// 模型
class Model : public Component
{
public:
	struct NodeDrawInfo
	{
		ModelNodePtr node;
		std::vector<int> meshes;
		bool visible;
	};

	Model();
	~Model();

	bool load(const std::string &path, ShaderProgramPtr shader);

	virtual void draw(Renderer *renderer) override;

	ModelNodePtr getRoot() const { return root_; }
	ModelNodePtr findNode(const std::string &name) const;

	void setNodeVisible(const std::string &name, bool visible);

protected:
	void applyMatrix(const Matrix &worldMatrix);

	std::string			resource_;
	std::vector<MeshPtr> meshes_;
	std::vector<NodeDrawInfo> drawInfo_;
	ModelNodePtr		root_;
	std::unordered_map<std::string, ModelNodePtr> nodeMap_;

	friend class ModelNodeLoader;
};

typedef SmartPointer<Model> ModelPtr;
