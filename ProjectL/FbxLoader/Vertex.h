#pragma once
#include <assimp/scene.h>
#include <assimp/mesh.h>

class Vertex
{
public:
	Vertex(const aiVector3D& position, const aiVector3D& normal, const aiVector2D& texCoords)
		:Position(position), Normal(normal), TexCoords(texCoords)
	{

	}

	aiVector3D GetPosition() const
	{
		return Position;
	}

	aiVector3D GetNormal() const
	{
		return Normal;
	}

	aiVector2D GetTexCoords() const
	{
		return TexCoords;
	}

private:
	aiVector3D Position;
	aiVector3D Normal;
	aiVector2D TexCoords;
};

