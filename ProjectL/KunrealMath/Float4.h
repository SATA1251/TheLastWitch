#pragma once
#include "CommonHeader.h"

namespace KunrealEngine
{
	namespace KunrealMath
	{
		struct Float4
		{
			float x;
			float y;
			float z;
			float w;

			Float4() = default;
			Float4(const Float4&) = default;
			Float4& operator=(const Float4&) = default;

			Float4(Float4&&) = default;
			Float4& operator=(Float4&&) = default;

			constexpr Float4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}

			Float4 operator+(const Float4& param) const
			{
				return Float4(x + param.x, y + param.y, z + param.z, w + param.w);
			}

			Float4 operator*(const Float4& param) const
			{
				return Float4(x * param.x, y * param.y, z * param.z, w * param.w);
			}

			bool operator==(const Float4& param) const
			{
				if (x == param.x && y == param.y && z == param.z && w == param.w)
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