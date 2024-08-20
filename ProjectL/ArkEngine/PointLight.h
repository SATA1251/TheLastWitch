/// <summary>
/// 
/// PointLight
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
		class PointLight
		{
		public:
			PointLight(DirectX::XMFLOAT4 ambient, DirectX::XMFLOAT4 diffuse, DirectX::XMFLOAT4 specular, DirectX::XMFLOAT3 position, float radius, float att);
			~PointLight();

		public:
			void SetAmbient(DirectX::XMFLOAT4& ambient);
			void SetDiffuse(DirectX::XMFLOAT4& diffuse);
			void SetSpecular(DirectX::XMFLOAT4& specular);


			void SetPosition(DirectX::XMFLOAT3 position);
			void SetRange(float radius);
			void SetAttenuation(float attnuation);

		private:
			DirectX::XMFLOAT4 _ambient;
			DirectX::XMFLOAT4 _diffuse;
			DirectX::XMFLOAT4 _specular;

			DirectX::XMFLOAT3 _position;
			float _radius;

			DirectX::XMFLOAT3 _att;
			

			float _attenuation;
			
		};
	}
}