#pragma once
#include <d3d11.h>
#include <dxgi.h>
#include <DirectXMath.h>
#include <wrl/client.h>

using namespace Microsoft::WRL;

namespace ArkEngine
{
	class ShadowMap
	{
	public:
		ShadowMap(ComPtr<ID3D11Device> device, int width, int height);
		~ShadowMap();

		ComPtr<ID3D11ShaderResourceView> DepthMapSRV();
		void BindDsvAndSetNullRenderTarget(ComPtr<ID3D11DeviceContext> deviceContext);

	private:
		ShadowMap(const ShadowMap& rhs);
		ShadowMap& operator = (const ShadowMap& rhs);

	private:
		int _width;
		int _height;

		ComPtr<ID3D11ShaderResourceView> _depthMapSRV;
		ComPtr<ID3D11DepthStencilView> _depthMapDSV;

		D3D11_VIEWPORT _viewport;
	};
}
