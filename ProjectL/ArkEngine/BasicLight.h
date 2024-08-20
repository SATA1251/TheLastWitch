/// <summary>
/// 
/// �������� ����Ʈ
/// 
/// ����ȭ
/// </summary>

#pragma once
/// �̰� �� �ȳ����� �������°��� ���ڱ�..
#include <DirectXMath.h>

namespace DirectX
{
	struct XMFLOAT4;
}

namespace ArkEngine
{
	namespace ArkDX11
	{
		class BasicLight
		{
		public:
			BasicLight(DirectX::XMFLOAT4 ambient, DirectX::XMFLOAT4 diffuse, DirectX::XMFLOAT4 specular);
			~BasicLight() {};

		public:
			void SetAmbient(DirectX::XMFLOAT4 ambient);
			void SetDiffuse(DirectX::XMFLOAT4 diffuse);
			void SetSpecular(DirectX::XMFLOAT4 specular);


		private:
			DirectX::XMFLOAT4 _ambient;
			DirectX::XMFLOAT4 _diffuse;
			DirectX::XMFLOAT4 _specular;
		};
	}
}


