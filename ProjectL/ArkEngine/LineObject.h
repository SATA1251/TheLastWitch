/// <summary>
/// 디버그용 라인을 그리기 위한 객체
/// 
/// 윤종화
/// </summary>

#pragma once
#include "d3d11.h"
#include "ILineObject.h"

struct ID3DX11Effect;
struct ID3D11Buffer;
struct ID3DX11EffectTechnique;
struct ID3DX11EffectMatrixVariable;

namespace DirectX
{
	struct XMFLOAT3;
}

namespace ArkEngine
{
	class ICamera;

	namespace ArkDX11
	{
		class ArkEffect;
		class ArkDevice;

		class LineObject : public ILineObject
		{
		public:
			LineObject();
			~LineObject();

		public:
			virtual void Initialize(const DirectX::XMFLOAT3& vertex1, const DirectX::XMFLOAT3& vertex2, const DirectX::XMFLOAT4& color) override;
			virtual void Update(ArkEngine::ICamera* p_Camera) override;
			virtual void Render() override;
			virtual void Release() override;

			virtual const DirectX::XMFLOAT3& GetVertex1() override;
			virtual const DirectX::XMFLOAT3& GetVertex2() override;

		private:
			void BuildGeometryBuffers();
			void SetEffect();

		private:
			DirectX::XMFLOAT3 _vertex1;
			DirectX::XMFLOAT3 _vertex2;

			DirectX::XMFLOAT4 _color;

			ArkEngine::ArkDX11::ArkEffect* _arkEffect;
			ID3DX11Effect* _effect;
			ArkEngine::ArkDX11::ArkDevice* _arkDevice;

			std::string _effectName;

		private:
			DirectX::XMFLOAT4X4 _world;
			DirectX::XMFLOAT4X4 _view;
			DirectX::XMFLOAT4X4 _proj;

		private:
			ID3D11Buffer* _vertexBuffer;
			ID3D11Buffer* _indexBuffer;

			ID3DX11EffectTechnique* _tech;

			ID3DX11EffectMatrixVariable* _fxWorld;
			ID3DX11EffectMatrixVariable* _fxWorldViewProj;
		};
	}
}

