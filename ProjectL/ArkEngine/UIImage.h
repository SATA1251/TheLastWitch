/// <summary>
/// UI를 위한
/// 피킹이 되는 이미지 오브젝트
/// 
/// 윤종화
/// </summary>

#pragma once
#include "IUIImage.h"
#include "GraphicsImage.h"

struct ID3D11Buffer;
struct ID3DX11Effect;
struct ID3DX11EffectTechnique;
struct ID3DX11EffectMatrixVariable;
struct ID3D11ShaderResourceView;
struct ID3DX11EffectShaderResourceVariable;

namespace ArkEngine
{
	class ICamera;

	namespace ArkDX11
	{
		class ArkDevice;
		class ArkBuffer;
		class ArkEffect;
		class Transform;

		class UIImage : public ArkEngine::IUIImage, public GInterface::GraphicsImage
		{
		public:
			UIImage(const std::string& imageName, unsigned int screenWidth, unsigned int screenHeight);
			~UIImage();

		public:
			virtual void Render(DirectX::DX11::SpriteBatch* sp) override;

		public:
			virtual bool GetRenderingState() override;
			virtual void SetRenderingState(bool tf) override;

		public:
			virtual const std::string& GetImageName() override;
			// 이미지 객체의 트랜스폼 설정	
			virtual void SetTransform(DirectX::XMFLOAT4X4 matrix) override;
			// 이미지 객체의 위치값 설정 (좌상단 0,0)
			virtual void SetPosition(float x = 0.0f, float y = 0.0f) override;
			// 이미지 객체의 회전값 설정
			virtual void SetRotation(float angle) override;
			// 이미지 객체의 크기값 설정
			virtual void SetScale(float x = 1.0f, float y = 1.0f) override;
			// 이미지의 알파값
			virtual void SetAlpha(float alpha) override;

			// 이미지 객체 삭제
			virtual void Delete() override;

			virtual const DirectX::XMUINT2& GetImageSize() override;

			virtual void ChangeImage(const std::string& imageName) override;

		private:
			void Initialize();
			
			void SetTexture();

			void ChangeTexture();

		private:
			std::string _imageName;

			bool _isRendering;
			float _alpha;

			DirectX::XMUINT2 _texturePos;
			DirectX::XMUINT2 _originTextureSize;
			DirectX::XMUINT2 _finalTextureSize;

			ID3D11ShaderResourceView* _textureSRV;
		};
	}
}


