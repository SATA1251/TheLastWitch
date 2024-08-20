/// <summary>
/// 
/// SpotLight
/// 
/// À±Á¾È­
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
		class SpotLight : public ArkEngine::ArkDX11::BasicLight
		{
		public:
			SpotLight(DirectX::XMFLOAT4 ambient, DirectX::XMFLOAT4 diffuse, DirectX::XMFLOAT4 specular, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 direction);
			~SpotLight();

		public:
			void SetPosition(DirectX::XMFLOAT3 position);
			void SetDir(DirectX::XMFLOAT3 direction);
			void SetRange(float range);

		private:
			DirectX::XMFLOAT3 _position;
			float _range;

			DirectX::XMFLOAT3 _direction;
			float _spot;

			DirectX::XMFLOAT3 _att;
			float _pad;
		};
	}
}
