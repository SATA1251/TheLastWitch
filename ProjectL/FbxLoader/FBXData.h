#pragma once
#include <iostream>
#include <vector>
#include <assimp/mesh.h>


struct Vertex
{
	aiVector3D position;		// 버텍스 위치
	aiVector3D normal;			// 버텍스 노말
	aiVector2D texCoord;		// 텍스쳐 좌표
};

struct Texture
{
	unsigned int id;
	std::string type;
};

struct Material
{
	aiVector3D diffuseColor;
	std::string diffuseTexturePath;
};

struct MeshInfo
{
	std::string name;
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;
};

struct Model
{
	std::vector<MeshInfo> meshes;
	Material material;
};


class FBXData
{
};

