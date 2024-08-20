#pragma once
#include "Mesh.h"

class Model
{
public:
	Model();
	~Model();

	void AddMesh(const Mesh& mesh)
	{
		meshes.push_back(mesh);
	}

	void AddMaterial();

private:
	std::vector<Mesh> meshes;
};

