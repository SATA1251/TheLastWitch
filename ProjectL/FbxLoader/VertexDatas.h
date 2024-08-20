#pragma once
#include "Types.h"



struct VertexData
{
	Vec3 position = { 0, 0, 0 };
};

struct VertexTextureData
{
	Vec3 position = { 0, 0, 0 };
	Vec2 uv = { 0, 0 };

	//static std::vector<D3D11_INPUT_ELEMENT_DESC> descs;
};

struct VertexColorData
{
	Vec3 position = { 0, 0, 0 };
	Color color = { 0, 0, 0, 0 };

	//static std::vector<D3D11_INPUT_ELEMENT_DESC> descs;
};

struct VertexTextureNormalData
{
	Vec3 position = { 0, 0, 0 };
	Vec2 uv = { 0, 0 };
	Vec3 normal = { 0, 0, 0 };
};

struct VertexTextureNormalTangentData
{
	Vec3 position = { 0, 0, 0 };
	Vec2 uv = { 0, 0 };
	Vec3 normal = { 0, 0, 0 };
	Vec3 tangent = { 0, 0, 0 };
};

struct VertexTextureNormalTangentBlendData
{

	DirectX::XMFLOAT3 position = { 0, 0, 0 };
	DirectX::XMFLOAT3 normal = { 0, 0, 0 };
	DirectX::XMFLOAT2 uv = { 0, 0 };
	DirectX::XMFLOAT3 tangent = { 0, 0, 0 };

	DirectX::XMFLOAT3 weight = { 0, 0, 0 };
	DirectX::XMUINT4 boneIndices = { 0, 0, 0, 0 };
};

//Vec3 position = { 0, 0, 0 };
//Vec2 uv = { 0, 0 };
//Vec3 normal = { 0, 0, 0 };
//Vec3 tangent = { 0, 0, 0 };
//Vec4 blendIndices = { 0, 0, 0, 0 };
//Vec3 blendWeights = { 0, 0, 0 };


using ModelVertexType = VertexTextureNormalTangentBlendData;
