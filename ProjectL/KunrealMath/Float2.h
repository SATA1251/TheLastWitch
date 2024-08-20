#pragma once
#include "CommonHeader.h"

namespace KunrealEngine
{
	namespace KunrealMath
	{
		struct Float2
		{
			float x;
			float y;

			Float2() = default;
			Float2(const Float2&) = default;
			Float2& operator=(const Float2&) = default;

			Float2(Float2&&) = default;
			Float2& operator=(Float2&&) = default;

			constexpr Float2(float _x, float _y) : x(_x), y(_y) {}

			bool operator==(const Float2& param) const
			{
				if (x == param.x && y == param.y)
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