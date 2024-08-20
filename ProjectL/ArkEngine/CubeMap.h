/// <summary>
/// 큐브맵을 적용시키기 위한 클래스
/// 
/// 윤종화
/// </summary>

#pragma once
#include "ICubeMap.h"

struct ID3DX11EffectMatrixVariable;
struct ID3DX11EffectShaderResourceVariable;
struct ID3D11ShaderResourceView;

namespace ArkEngine
{
	class ICamera;
};

namespace ArkEngine
{
	namespace ArkDX11
	{
		class CubeMap : public ArkEngine::ICubeMap
		{
		public:
			CubeMap(const std::string& fileName, const std::string& textureName = "", bool isCube = false);
			~CubeMap();

		public:
			void Initialize();
			void Finailize();

		public:
			virtual void Update(ArkEngine::ICamera* p_Camera) override;
			virtual void Render() override;
			virtual const std::string& GetName() override;
			virtual const std::string& GetTextureName() override;

		private:
			void BuildGeometryBuffers();
			void SetEffect();
			void SetTexture(const char* textureName);

		private:
			std::string _fileName;
			std::string _effectName;
			std::string _textureName;

			ID3DX11Effect* _effect;
			ID3DX11EffectTechnique* _tech;

			ID3DX11EffectMatrixVariable* _fxWorldViewProj;

			ID3DX11EffectShaderResourceVariable* _cubeMap;
			ID3D11ShaderResourceView* _cubeMapSRV;

			ID3D11Buffer* _vertexBuffer;
			ID3D11Buffer* _indexBuffer;

			DirectX::XMFLOAT4X4 _world;
			DirectX::XMFLOAT4X4 _view;
			DirectX::XMFLOAT4X4 _proj;

		private:
			ArkEngine::ArkDX11::ArkDevice* _arkDevice;
			ArkEngine::ArkDX11::ArkEffect* _arkEffect;

		private:
			unsigned int _totalIndexCount;
			bool _isCube;
		};
	}
}

