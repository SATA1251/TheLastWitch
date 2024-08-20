/// <summary>
/// 텍스쳐를 만들기 위한
/// ID3D11Resource와 ID3D11ShaderResourceView를
/// IResource로 상속받기 위해 래핑한 클래스
/// 
/// 윤종화
/// </summary>

#pragma once
#include <DirectXMath.h>
#include "IResource.h"

struct ID3D11Resource;
struct ID3D11ShaderResourceView;

namespace ArkEngine
{
	namespace ArkDX11
	{
		class ArkTexture : public ArkEngine::IResource
		{
		public:
			ArkTexture(const char* textureName);
			~ArkTexture();

		public:
			virtual void Load(const char* fileName) override;
			virtual void UnLoad(const char* fileName) override;

		public:
			ID3D11ShaderResourceView* GetDiffuseMapSRV();

		private:
			void CreateDDSTexture(const char* textureName);

		private:
			void CreateTexture(const char* textureName);

		private:
			void CreateTGATexture(const char* textureName);

		private:
			ID3D11ShaderResourceView* _diffuseMapSRV;

		public:
			const DirectX::XMUINT2& GetTextureSize();

		private:
			DirectX::XMUINT2 _textureSize;
		};
	}
}





