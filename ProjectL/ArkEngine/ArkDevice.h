/// <summary>
/// ID3D11Device�� ID3D11DeviceContext��
/// IResource�� ��ӹޱ� ���� ������ Ŭ����
/// 
/// ����ȭ
/// </summary>

#pragma once
#include "IResource.h"

struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11DepthStencilView;
struct ID3D11ShaderResourceView;

namespace ArkEngine
{
	namespace ArkDX11
	{
		class ArkDevice : public ArkEngine::IResource
		{
		public:
			ArkDevice(ID3D11Device* p_device, ID3D11DeviceContext* p_deviceContext, ID3D11DepthStencilView* p_depthStencilView, ID3D11RasterizerState* p_SolidRS, ID3D11RasterizerState* P_WireRS, ID3D11RasterizerState* P_ShadowRS);
			~ArkDevice();

		public:
			virtual void Load(const char* fileName) override;
			virtual void UnLoad(const char* fileName) override;

		public:
			ID3D11Device* GetDevice();
			ID3D11DeviceContext* GetDeviceContext();
			ID3D11DepthStencilView* GetDepthStencilView();
			ID3D11RasterizerState* GetSolidRS();
			ID3D11RasterizerState* GetWireRS();
			ID3D11RasterizerState* GetShadowRS();
			ID3D11ShaderResourceView* GetRandomTex();

		public:
			void SetRandomTex(ID3D11ShaderResourceView* randomTex);


		private:
			ID3D11Device* _device;
			ID3D11DeviceContext* _deviceContext;
			ID3D11DepthStencilView* _depthStencilView;

			ID3D11RasterizerState* _solidRS;
			ID3D11RasterizerState* _wireRS;
			ID3D11RasterizerState* _shadowRS;

			// ��ƼŬ ����
			// ���ҽ� �Ŵ������� �����ؾ� �ұ� �ϴٰ� �����ϱ� �Ⱦ...
			ID3D11ShaderResourceView* _randomTexSRV;
		};
	}
}

