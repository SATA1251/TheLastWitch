#pragma once
#include "CommonHeader.h"

namespace KunrealEngine
{
	namespace KunrealMath
	{
		struct Matrix3x3
		{
			union
			{
				struct
				{
					float _11, _12, _13;
					float _21, _22, _23;
					float _31, _32, _33;
				};
				float matrix[3][3];
			};

			Matrix3x3() = default;
			Matrix3x3(const Matrix3x3&) = default;
			Matrix3x3& operator=(const Matrix3x3&) = default;

			Matrix3x3(Matrix3x3&&) = default;
			Matrix3x3& operator=(Matrix3x3&&) = default;

			constexpr Matrix3x3(float m00, float m01, float m02,
								float m10, float m11, float m12,
								float m20, float m21, float m22)
				: _11(m00), _12(m01), _13(m02),
				_21(m10), _22(m11), _23(m12),
				_31(m20), _32(m21), _33(m22) 
			{
				matrix[0][0] = m00;
				matrix[0][1] = m01;
				matrix[0][2] = m02;

				matrix[1][0] = m10;
				matrix[1][1] = m11;
				matrix[1][2] = m12;

				matrix[2][0] = m20;
				matrix[2][1] = m21;
				matrix[2][2] = m22;
			}
		};
	}
}