/// <summary>
/// ID3D11Buffer를
/// IResource로 상속받기 위해 래핑한 클래스
/// 
/// 윤종화
/// </summary>

#pragma once
#include <vector>
#include "IResource.h"

namespace DirectX
{
	struct XMFLOAT3;
}

struct ID3D11Buffer;

namespace ArkEngine
{
	namespace ArkDX11
	{
		struct Vertex;
		struct Postex;
		struct PosColor;

		class ArkBuffer : public ArkEngine::IResource
		{
		public:
			ArkBuffer(const char* fileName, int totalVertexCount, const std::vector<ArkEngine::ArkDX11::Vertex>& vertexList, int totalIndexCount, const std::vector<unsigned int>& indexList);
			ArkBuffer(const char* fileName, int totalVertexCount, const std::vector<ArkEngine::ArkDX11::Postex>& vertexList, int totalIndexCount, const std::vector<unsigned int>& indexList);
			ArkBuffer(const char* fileName, int totalVertexCount, const std::vector<ArkEngine::ArkDX11::PosColor>& vertexList, int totalIndexCount, const std::vector<unsigned int>& indexList);

			~ArkBuffer();

		public:
			ID3D11Buffer* GetVertexBuffer();
			ID3D11Buffer* GetIndexBuffer();

			unsigned int const GetTotalVertexCount();
			unsigned int const GetTotalIndexCount();

		public:
			const std::vector<DirectX::XMFLOAT3>& GetVertexPosList();
			const std::vector<unsigned int>& GetIndexList();

			const DirectX::XMFLOAT3& GetMaxPos();
			const DirectX::XMFLOAT3& GetMinPos();

			void SetSize(DirectX::XMFLOAT3 minPos, DirectX::XMFLOAT3 maxPos);

		private:
			void CreateVertexBuffer(int totalVertexCount, std::vector<ArkEngine::ArkDX11::Vertex> vertexList);
			void CreatePosTexBuffer(int totalVertexCount, std::vector<ArkEngine::ArkDX11::Postex> vertexList);
			void CreatePosColorBuffer(int totalVertexCount, std::vector<ArkEngine::ArkDX11::PosColor> vertexList);

			void CreateIndexBuffer(int totalIndexCount, std::vector<unsigned int> indexList);

		public:
			virtual void Load(const char* fileName) override;
			virtual void UnLoad(const char* fileName) override;

		private:
			ID3D11Buffer* _vertexBuffer;
			ID3D11Buffer* _indexBuffer;

			unsigned int _totalVertexCount;
			unsigned int _totalIndexCount;

			DirectX::XMFLOAT3 _minPos;
			DirectX::XMFLOAT3 _maxPos;

			std::vector<DirectX::XMFLOAT3> _vertexPosList;
			std::vector<unsigned int> _indexList;

		};
	}
}