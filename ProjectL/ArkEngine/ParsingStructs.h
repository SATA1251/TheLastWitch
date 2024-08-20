#pragma once
#include <iostream>
#include <vector>
#include "CommonStruct.h"
#include "../FbxLoader/Types.h"
#include "../FbxLoader/VertexDatas.h"
#include "Geometry.h"
#include "DirectXMath.h"

struct ModelBone
{
	std::wstring name = L"";
	int index = 0;
	int parentIndex = 0;
	std::shared_ptr<ModelBone> parent = nullptr;	// Cache

	Matrix transform = {};
	std::vector<std::shared_ptr<ModelBone>> children = {0};	// Cache

	Matrix boneOffsetTM = {};

	//DirectX::XMFLOAT4X4 OriginWorldTM;
	//DirectX::XMFLOAT4X4 LocalTM;
	//DirectX::XMFLOAT4X4 NewWorldTM;

	//DirectX::XMFLOAT4X4 OriginLocalTM;

	//DirectX::XMFLOAT4X4 SlerpTM;

};

struct ModelMaterial
{
	std::vector<std::string> _diffuseTextureList;
	std::vector<std::string> _normalTextureList;
	std::vector<std::string> _emissiveTextureList;
	std::vector<ArkEngine::ArkDX11::Material> _material;
};

struct ModelMesh
{
	// Bones
	std::shared_ptr<ModelBone> bone;
	std::vector<std::shared_ptr<ModelBone>> _boneList;


	// Mesh
	std::wstring name = L"";
	int boneIndex = 0;

	unsigned int vertexNum = 0;
	std::vector<ModelVertexType> vertices;

	std::shared_ptr<Geometry<ModelVertexType>> _geometry = std::make_shared<Geometry<ModelVertexType>>();

	unsigned int indexNum = 0;

	std::vector<unsigned int> indices;

	
	// Material
	std::wstring materialName = L"";
	std::unique_ptr<ModelMaterial> material;
};

