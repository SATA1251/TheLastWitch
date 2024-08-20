#pragma once
#include <vector>
#include "Vertex.h"

class Mesh
{
public:
	Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices)
		: vertices(vertices), indices(indices)
	{}
	~Mesh();



private:
	std::vector<Vertex> vertices;		// �޽��� vertex ������
	std::vector<unsigned int> indices;	// �޽��� index ������
};