/// <summary>
/// 
/// 그래픽스 엔진에 쓰이는 공용 구조체들
/// 
/// 윤종화
/// </summary>

#pragma once
#include <DirectXMath.h>

namespace DirectX
{
	struct XMFLOAT2;
	struct XMFLOAT3;
	struct XMFLOAT4;
	struct XMUINT4;
}

namespace ArkEngine
{
	namespace ArkDX11
	{
		struct Postex
		{
			DirectX::XMFLOAT3 pos;
			DirectX::XMFLOAT2 tex;
		};

		struct Vertex
		{
			DirectX::XMFLOAT3 pos;
			DirectX::XMFLOAT3 norm;
			DirectX::XMFLOAT2 tex;
			DirectX::XMFLOAT3 tangent;
			// 스키닝에서 추가함
			DirectX::XMFLOAT3 weight;

			DirectX::XMUINT4 boneIndicle;
		};

		struct PosColor
		{
			DirectX::XMFLOAT3 pos;
			DirectX::XMFLOAT4 color;
		};

		struct Material
		{
			DirectX::XMFLOAT4 ambient;
			DirectX::XMFLOAT4 diffuse;
			DirectX::XMFLOAT4 specular;
			DirectX::XMFLOAT4 reflect;
		};
	}
}

