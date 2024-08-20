/// <summary>
/// 
/// KunrealMath를 DirectXMath로 컨버팅
/// mesh, camera, dx11renderer에서 사용되기에
/// 중복되어 lnk2005가 발생 -> inline 키워드로 일단 해결
/// 
/// 윤종화
/// </summary>

#pragma once
#include <DirectXMath.h>
#include "../KunrealMath/MathHeaders.h"

namespace ArkEngine
{
	namespace ArkDX11
	{
		inline DirectX::XMFLOAT3 ConvertFloat3(const KunrealEngine::KunrealMath::Float3& target)
		{
			DirectX::XMFLOAT3 result;

			result.x = target.x;
			result.y = target.y;
			result.z = target.z;

			return result;
		}

		inline DirectX::XMFLOAT4 ConvertFloat4(const KunrealEngine::KunrealMath::Float4& target)
		{
			DirectX::XMFLOAT4 result;

			result.x = target.x;
			result.y = target.y;
			result.z = target.z;
			result.w = target.w;

			return result;
		}

		inline DirectX::XMFLOAT4X4 ConvertMatrix4x4(const KunrealEngine::KunrealMath::Matrix4x4& target)
		{
			DirectX::XMFLOAT4X4 result;

			result._11 = target._11;		result._12 = target._12;		result._13 = target._13;		result._14 = target._14;
			result._21 = target._21;		result._22 = target._22;		result._23 = target._23;		result._24 = target._24;
			result._31 = target._31;		result._32 = target._32;		result._33 = target._33;		result._34 = target._34;
			result._41 = target._41;		result._42 = target._42;		result._43 = target._43;		result._44 = target._44;

			return result;
		}
	}
}