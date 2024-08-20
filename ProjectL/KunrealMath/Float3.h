#pragma once
#include "CommonHeader.h"

/// <summary>
/// Float형 변수 3개로 이루어진 구조체
/// 가장 많이 사용될 구조체이므로 연산자 오버로딩 등 신경을 쓴 부분
/// 
/// 이건호
/// </summary>

namespace KunrealEngine
{
	namespace KunrealMath
	{
		struct Float3
		{
			float x;
			float y;
			float z;

			Float3() = default;
			Float3(const Float3& param) = default;
			Float3& operator=(const Float3&) = default;

			Float3(Float3&& param) = default;
			Float3& operator=(Float3&&) = default;

			// 이 부분이 있어야 밑에 연산자 오버로딩이 가능
			// DXMath에서는 XM_CONSTEXPR이라는 매크로로 되어있음
			constexpr Float3(float _x, float _y, float _z) noexcept : x(_x), y(_y), z(_z) {}


			// 연산자 오버로딩 부분
			Float3 operator+(const Float3& param) const
			{
				return Float3(x + param.x, y + param.y, z + param.z);
			}

			Float3 operator-(const Float3& param) const
			{
				return Float3(x - param.x, y - param.y, z - param.z);
			}

			Float3 operator-() const
			{
				return Float3(-x, -y, -z);
			}

			Float3 operator*(float scalar) const
			{
				return Float3(x * scalar, y * scalar, z * scalar);
			}

			Float3 operator*(const Float3& param) const
			{
				return Float3(x * param.x, y * param.y, z * param.z);
			}

			bool operator==(const Float3& param) const
			{
				if (x == param.x && y == param.y && z == param.z)
				{
					return true;
				}
				else
				{
					return false;
				}
			}
		};
	}
}