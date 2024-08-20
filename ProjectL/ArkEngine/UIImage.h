/// <summary>
/// UI�� ����
/// ��ŷ�� �Ǵ� �̹��� ������Ʈ
/// 
/// ����ȭ
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
			// �̹��� ��ü�� Ʈ������ ����	
			virtual void SetTransform(DirectX::XMFLOAT4X4 matrix) override;
			// �̹��� ��ü�� ��ġ�� ���� (�»�� 0,0)
			virtual void SetPosition(float x = 0.0f, float y = 0.0f) override;
			// �̹��� ��ü�� ȸ���� ����
			virtual void SetRotation(float angle) override;
			// �̹��� ��ü�� ũ�Ⱚ ����
			virtual void SetScale(float x = 1.0f, float y = 1.0f) override;
			// �̹����� ���İ�
			virtual void SetAlpha(float alpha) override;

			// �̹��� ��ü ����
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


