/// <summary>
/// �׷��Ƚ� �������� ����ϴ�
/// 
/// 
/// ����ȭ
/// </summary>

#pragma once
#include <DirectXMath.h>

namespace GInterface
{
	struct Material
	{
		DirectX::XMFLOAT4 ambient;
		DirectX::XMFLOAT4 diffuse;
		DirectX::XMFLOAT4 specular;
		DirectX::XMFLOAT4 reflect;
	};
}
