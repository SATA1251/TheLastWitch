#pragma once
/// <summary>
/// Vertex 정보와 Index 정보를 묶어서 관리할 수 있는 클래스
/// </summary>
/// <typeparam name="T"></typeparam>
#include <vector>

template<typename T>
class Geometry
{
public:
	Geometry() {}
	~Geometry() {}

	unsigned int GetVertexCount() { return static_cast<unsigned int>(_vertices.size()); }
	void* GetVertexData() { return _vertices.data(); }
	const std::vector<T>& GetVertices() { return _vertices; }

	unsigned int GetIndexCount() { return static_cast<unsigned int>(_indices.size()); }
	void* GetIndexData() { return _indices.data(); }
	const std::vector<unsigned int>& GetIndices() { return _indices; }

	// Vertex 정보를 담아둔다
	void AddVertex(const T& vertex) { _vertices.push_back(vertex); }
	void AddVertices(const std::vector<T>& vertices) { _vertices.insert(_vertices.end(), vertices.begin(), vertices.end()); }
	void SetVertices(const std::vector<T>& vertices) { _vertices = vertices; }

	// Index 정보를 담아둔다
	void AddIndex(unsigned int index) { _indices.push_back(index); }
	void AddIndices(const std::vector<unsigned int>& indices) { _indices.insert(_indices.end(), indices.begin(), indices.end()); }
	void SetIndices(const std::vector<unsigned int>& indices) { _indices = indices; }

private:
	// vertex 정보를 채운다
	std::vector<T> _vertices;

	// index 정보를 채운다
	std::vector<unsigned int> _indices;
};


