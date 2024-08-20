#pragma once

namespace DirectX
{
	struct XMFLOAT3;
}

namespace ArkEngine
{
	namespace ArkDX11
	{
		class GeometryGenerator
		{
		public:
			void CreateQuad();
			void CreateFlatQuad();
			void Create3dQuad();
			void CreateBox(const char* geometryName, float width, float height, float depth);
			void CreateSphere(const char* geometryName, float radius, unsigned int sliceCount, unsigned int stackCount);
			void CreateCircle(const char* geometryName);

			void CreateDebugBox(const char* geometryName, const DirectX::XMFLOAT3& centerPos, float width, float height, float depth, const DirectX::XMFLOAT4& color);
			void CreateDebugPhysXBox(const char* geometryName, const DirectX::XMFLOAT3& centerPos, float width, float height, float depth, const DirectX::XMFLOAT4& color);
			void CreateDebugSphere(const char* geometryName, float minPos, float range, const DirectX::XMFLOAT4& color);
		};
	}
}

