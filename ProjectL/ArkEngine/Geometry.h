#pragma once
/// <summary>
/// Vertex ������ Index ������ ��� ������ �� �ִ� Ŭ����
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

	// Vertex ������ ��Ƶд�
	void AddVertex(const T& vertex) { _vertices.push_back(vertex); }
	void AddVertices(const std::vector<T>& vertices) { _vertices.insert(_vertices.end(), vertices.begin(), vertices.end()); }
	void SetVertices(const std::vector<T>& vertices) { _vertices = vertices; }

	// Index ������ ��Ƶд�
	void AddIndex(unsigned int index) { _indices.push_back(index); }
	void AddIndices(const std::vector<unsigned int>& indices) { _indices.insert(_indices.end(), indices.begin(), indices.end()); }
	void SetIndices(const std::vector<unsigned int>& indices) { _indices = indices; }

private:
	// vertex ������ ä���
	std::vector<T> _vertices;

	// index ������ ä���
	std::vector<unsigned int> _indices;
};


