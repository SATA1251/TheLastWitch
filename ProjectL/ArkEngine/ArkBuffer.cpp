#include "ResourceManager.h"
#include "CommonStruct.h"
#include "ArkDevice.h"
#include "ArkBuffer.h"

ArkEngine::ArkDX11::ArkBuffer::ArkBuffer(const char* fileName, int totalVertexCount, const std::vector<ArkEngine::ArkDX11::Vertex>& vertexList, int totalIndexCount, const std::vector<UINT>& indexList)
	: _vertexBuffer(nullptr), _indexBuffer(nullptr), _totalVertexCount(totalVertexCount), _totalIndexCount(totalIndexCount), _minPos(), _maxPos()
{
	CreateVertexBuffer(totalVertexCount, vertexList);
	CreateIndexBuffer(totalIndexCount, indexList);

	for (int i = 0; i < vertexList.size(); i++)
	{
		_vertexPosList.emplace_back(vertexList[i].pos);
	}
	for (int i = 0; i < indexList.size(); i++)
	{
		_indexList.emplace_back(indexList[i]);
	}

	Load(fileName);
}

ArkEngine::ArkDX11::ArkBuffer::ArkBuffer(const char* fileName, int totalVertexCount, const std::vector<ArkEngine::ArkDX11::PosColor>& vertexList, int totalIndexCount, const std::vector<unsigned int>& indexList)
	: _vertexBuffer(nullptr), _indexBuffer(nullptr), _totalVertexCount(totalVertexCount), _totalIndexCount(totalIndexCount), _minPos(), _maxPos()
{
	CreatePosColorBuffer(totalVertexCount, vertexList);
	CreateIndexBuffer(totalIndexCount, indexList);

	for (int i = 0; i < vertexList.size(); i++)
	{
		_vertexPosList.emplace_back(vertexList[i].pos);
	}
	for (int i = 0; i < indexList.size(); i++)
	{
		_indexList.emplace_back(indexList[i]);
	}

	Load(fileName);
}

ArkEngine::ArkDX11::ArkBuffer::ArkBuffer(const char* fileName, int totalVertexCount, const std::vector<ArkEngine::ArkDX11::Postex>& vertexList, int totalIndexCount, const std::vector<unsigned int>& indexList)
	:_vertexBuffer(nullptr), _indexBuffer(nullptr), _totalVertexCount(totalVertexCount), _totalIndexCount(totalIndexCount), _minPos(), _maxPos()
{
	CreatePosTexBuffer(totalVertexCount, vertexList);
	CreateIndexBuffer(totalIndexCount, indexList);

	for (int i = 0; i < vertexList.size(); i++)
	{
		_vertexPosList.emplace_back(vertexList[i].pos);
	}
	for (int i = 0; i < indexList.size(); i++)
	{
		_indexList.emplace_back(indexList[i]);
	}

	Load(fileName);
}

ArkEngine::ArkDX11::ArkBuffer::~ArkBuffer()
{
	_vertexBuffer->Release();
	_vertexBuffer = nullptr;
	
	_indexBuffer->Release();
	_indexBuffer = nullptr;
}

ID3D11Buffer* ArkEngine::ArkDX11::ArkBuffer::GetVertexBuffer()
{
	return _vertexBuffer;
}

ID3D11Buffer* ArkEngine::ArkDX11::ArkBuffer::GetIndexBuffer()
{
	return _indexBuffer;
}

const unsigned int ArkEngine::ArkDX11::ArkBuffer::GetTotalVertexCount()
{
	return _totalVertexCount;
}

const unsigned int ArkEngine::ArkDX11::ArkBuffer::GetTotalIndexCount()
{
	return _totalIndexCount;
}

const std::vector<DirectX::XMFLOAT3>& ArkEngine::ArkDX11::ArkBuffer::GetVertexPosList()
{
	return _vertexPosList;
}

const std::vector<unsigned int>& ArkEngine::ArkDX11::ArkBuffer::GetIndexList()
{
	return _indexList;
}

const DirectX::XMFLOAT3& ArkEngine::ArkDX11::ArkBuffer::GetMaxPos()
{
	return _maxPos;
}

const DirectX::XMFLOAT3& ArkEngine::ArkDX11::ArkBuffer::GetMinPos()
{
	return _minPos;
}

void ArkEngine::ArkDX11::ArkBuffer::SetSize(DirectX::XMFLOAT3 minPos, DirectX::XMFLOAT3 maxPos)
{
	_minPos = minPos;
	_maxPos = maxPos;
}

void ArkEngine::ArkDX11::ArkBuffer::CreateVertexBuffer(int totalVertexCount, std::vector<ArkEngine::ArkDX11::Vertex> vertexList)
{
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDesc.ByteWidth = sizeof(ArkEngine::ArkDX11::Vertex) * totalVertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &vertexList[0];

	auto device = ResourceManager::GetInstance()->GetResource<ArkEngine::ArkDX11::ArkDevice>("Device")->GetDevice();
	device->CreateBuffer(&vertexBufferDesc, &vinitData, &_vertexBuffer);
}

void ArkEngine::ArkDX11::ArkBuffer::CreatePosTexBuffer(int totalVertexCount, std::vector<ArkEngine::ArkDX11::Postex> vertexList)
{
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDesc.ByteWidth = sizeof(ArkEngine::ArkDX11::Postex) * totalVertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &vertexList[0];

	auto device = ResourceManager::GetInstance()->GetResource<ArkEngine::ArkDX11::ArkDevice>("Device")->GetDevice();
	device->CreateBuffer(&vertexBufferDesc, &vinitData, &_vertexBuffer);
}


void ArkEngine::ArkDX11::ArkBuffer::CreatePosColorBuffer(int totalVertexCount, std::vector<ArkEngine::ArkDX11::PosColor> vertexList)
{
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDesc.ByteWidth = sizeof(ArkEngine::ArkDX11::PosColor) * totalVertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &vertexList[0];

	auto device = ResourceManager::GetInstance()->GetResource<ArkEngine::ArkDX11::ArkDevice>("Device")->GetDevice();
	device->CreateBuffer(&vertexBufferDesc, &vinitData, &_vertexBuffer);
}

void ArkEngine::ArkDX11::ArkBuffer::CreateIndexBuffer(int totalIndexCount, std::vector<UINT> indexList)
{
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	indexBufferDesc.ByteWidth = sizeof(UINT) * totalIndexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &indexList[0];

	auto device = ResourceManager::GetInstance()->GetResource<ArkEngine::ArkDX11::ArkDevice>("Device")->GetDevice();
	device->CreateBuffer(&indexBufferDesc, &iinitData, &_indexBuffer);
}

void ArkEngine::ArkDX11::ArkBuffer::Load(const char* fileName)
{
	ResourceManager::GetInstance()->SetResource(fileName, this);
}

void ArkEngine::ArkDX11::ArkBuffer::UnLoad(const char* fileName)
{
	ResourceManager::GetInstance()->DeleteResource(fileName);
}
