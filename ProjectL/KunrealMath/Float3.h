#pragma once
#include "CommonHeader.h"

/// <summary>
/// Float�� ���� 3���� �̷���� ����ü
/// ���� ���� ���� ����ü�̹Ƿ� ������ �����ε� �� �Ű��� �� �κ�
/// 
/// �̰�ȣ
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

			// �� �κ��� �־�� �ؿ� ������ �����ε��� ����
			// DXMath������ XM_CONSTEXPR�̶�� ��ũ�η� �Ǿ�����
			constexpr Float3(float _x, float _y, float _z) noexcept : x(_x), y(_y), z(_z) {}


			// ������ �����ε� �κ�
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