#pragma once

namespace DirectX
{
	struct XMFLOAT3;
}

namespace ArkEngine
{
	class ICamera;
}

namespace ArkEngine
{
	class ILineObject abstract
	{
	public:
		ILineObject() {};
		virtual ~ILineObject() {};

	public:
		virtual void Initialize(const DirectX::XMFLOAT3& vertex1, const DirectX::XMFLOAT3& vertex2, const DirectX::XMFLOAT4& color) abstract;

		virtual void Update(ArkEngine::ICamera* camera) abstract;

		virtual void Render() abstract;

		virtual void Release() abstract;

		virtual const DirectX::XMFLOAT3& GetVertex1() abstract;
		virtual const DirectX::XMFLOAT3& GetVertex2() abstract;
	};

}