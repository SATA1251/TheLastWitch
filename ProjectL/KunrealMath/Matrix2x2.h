#pragma once
#include "CommonHeader.h"

namespace KunrealEngine
{
	namespace KunrealMath
	{
		struct Matrix2x2
		{
			union
			{
				struct
				{
					float _11, _12;
					float _21, _22;
				};
				float matrix[2][2];
			};

			Matrix2x2() = default;
			Matrix2x2(const Matrix2x2&) = default;
			Matrix2x2& operator=(const Matrix2x2&) = default;

			Matrix2x2(Matrix2x2&&) = default;
			Matrix2x2& operator=(Matrix2x2&&) = default;

			constexpr Matrix2x2(float m00, float m01, float m10, float m11)
				: _11(m00), _12(m01), _21(m10), _22(m11)
			{
				matrix[0][0] = m00;
				matrix[0][1] = m01;
				matrix[1][0] = m10;
				matrix[1][1] = m11;
			}
		};
	}
}