/// <summary>
/// 
/// 그림자 생성을 위한 클래스
/// 그림자의 깊이값을 측정하는 버퍼와
/// 그림자 맵을 위한 버퍼를 생성
/// 
/// 윤종화
/// </summary>


#pragma once
struct ID3D11Texture2D;
struct ID3D11DepthStencilView;
struct ID3D11ShaderResourceView;
struct ID3D11RenderTargetView;

namespace ArkEngine
{
	namespace ArkDX11
	{
		class ArkDevice;

		class ShadowBuffer
		{
		public:
			ShadowBuffer(int width, int height);
			~ShadowBuffer();

		public:
			void Initialize();
			void Finalize();

		public:
			void SetDepthStencilView();
			void ClearDepthStencilView();

			ID3D11ShaderResourceView* GetDepthSRV();
			ID3D11DepthStencilView* GetDepthStencilView();
			ID3D11Texture2D* GetTexture2D();

		private:
			void CreateDepthTexture(ArkEngine::ArkDX11::ArkDevice* pDeivce);
			void CreateDepthStencilView(ArkEngine::ArkDX11::ArkDevice* pDeivce);
			void CreateShaderResourceView(ArkEngine::ArkDX11::ArkDevice* pDeivce);
			
		private:
			int _textureWidth;
			int _textureHeight;
			float _color[4];

			ID3D11Texture2D* _depthTexture;
			ID3D11DepthStencilView* _depthStencilView;
			ID3D11ShaderResourceView* _shaderResourceView;
		};
	}
}

