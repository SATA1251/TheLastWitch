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
	std::vector<Vertex> vertices;		// 메시의 vertex 데이터
	std::vector<unsigned int> indices;	// 메시의 index 데이터
};