/// <summary>
/// 디퍼드 렌더링을 위한 
/// 디퍼드 버퍼를 생성하기 위한 클래스
/// 
/// 윤종화
/// </summary>

#pragma once
#include <vector>

struct ID3D11Texture2D;
struct ID3D11RenderTargetView;
struct ID3D11ShaderResourceView;
struct ID3D11DepthStencilView;


// GBuffer에 들어갈 요소들
enum class eBUFFERTYPE
{
	GBUFFER_POSITION,
	GBUFFER_DIFFUSE,
	GBUFFER_NORMALMAP,
	GBUFFER_EMISSIONMAP,
	GBUFFER_MATERIAL,
	GBUFFER_ADDITIONALINFO,
	GBUFFER_COLOR,
	GBUFFER_DECALPOS,
	// G버퍼 요소의 총 갯수
	GBUFFER_COUNT		
};

namespace ArkEngine
{
	namespace ArkDX11
	{
		class ArkDevice;

		class deferredBuffer
		{
		public:
			deferredBuffer(int width, int height);
			~deferredBuffer();

		public:
			void Initialize();
			void Finalize();

		public:
			void SetRenderTargets();
			void SetRenderTargets(ID3D11DepthStencilView* dsv);
			void ClearRenderTargets(float color[4]);
			void ClearRenderTargets(int index, float color[4]);

		public:
			void SetRenderTargetsForFinal();
			void ClearRenderTargetsForFinal(float color[4]);

		public:
			void SetRenderTargetsForBloom(int index);
			void ClearRenderTargetsForBloom(int index, float color[4]);

		public:
			ID3D11ShaderResourceView* GetSRV(int index);
			ID3D11Texture2D* GetTextrue(int index);
			ID3D11RenderTargetView* GetRenderTargetView(int index);

		public:
			ID3D11ShaderResourceView* GetSRVForFinal(int index);
			ID3D11Texture2D* GetTextrueForFinal(int index);
			ID3D11RenderTargetView* GetRenderTargetViewForFinal(int index);

		public:
			std::vector<ID3D11ShaderResourceView*>& GetSRVForBloomVec();
			ID3D11ShaderResourceView* GetSRVForBloom(int index);
			ID3D11Texture2D* GetTextrueForBloom(int index);
			ID3D11RenderTargetView* GetRenderTargetViewForBloom(int index);

		private:
			void CreateRenderTargetTexture();
			void CreateRenderTargetView();
			void CreateShaderResourceView();

		private:
			void CreateRenderTargetTextureForFinal();
			void CreateRenderTargetViewForFinal();
			void CreateShaderResourceViewForFinal();

		private:
			void CreateRenderTargetTextureForBloom();
			void CreateRenderTargetViewForBloom();
			void CreateShaderResourceViewForBloom();

		private:
			ArkDevice* _arkDevice;
			int _textureWidth;
			int _textureHeight;

			// texture를 또 사용할 일이 없으면 srv까지 만든 후에 release 시키고 멤버로도 가지고 있지 않을 예정
			ID3D11Texture2D* _renderTargetTextureArray[static_cast<int>(eBUFFERTYPE::GBUFFER_COUNT)];
			ID3D11RenderTargetView* _renderTargetViewArray[static_cast<int>(eBUFFERTYPE::GBUFFER_COUNT)];
			ID3D11ShaderResourceView* _shaderResourceViewArray[static_cast<int>(eBUFFERTYPE::GBUFFER_COUNT)];

		private:
			ID3D11Texture2D* _renderTargetTextureArrayForFinal;
			ID3D11RenderTargetView* _renderTargetViewArrayForFinal;
			ID3D11ShaderResourceView* _shaderResourceViewArrayForFinal;

		private:
			std::vector<ID3D11Texture2D*> _renderTargetTextureArrayForBloom;
			std::vector<ID3D11RenderTargetView*> _renderTargetViewArrayForBloom;
			std::vector<ID3D11ShaderResourceView*> _shaderResourceViewArrayForBloom;
		};
	}
}