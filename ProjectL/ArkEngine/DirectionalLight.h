/// <summary>
/// 
/// �ȼ� ���̴��� ��������
/// �������� DirectionalLight
/// 
/// ����ȭ
/// </summary>

#pragma once
#include "BasicLight.h"

namespace DirectX
{
	struct XMFLOAT3;
	struct XMFLOAT4;
}

namespace ArkEngine
{
	namespace ArkDX11
	{
		class DirectionalLight : public ArkEngine::ArkDX11::BasicLight
		{
		public:
			DirectionalLight(DirectX::XMFLOAT4 ambient, DirectX::XMFLOAT4 diffuse, DirectX::XMFLOAT4 specular, DirectX::XMFLOAT3 direction);
			~DirectionalLight();

		public:
			void SetDir(DirectX::XMFLOAT3 direction);

			DirectX::XMFLOAT3 GetDir();
		
		private:
			DirectX::XMFLOAT3 _direction;
			float _pad;
		};
	}
}