#pragma once

#include <emmintrin.h>
#include "CommonHeader.h"

namespace KunrealEngine
{
	namespace KunrealMath
	{
		struct Matrix4x4
		{
			union
			{
				struct
				{
					float _11, _12, _13, _14;
					float _21, _22, _23, _24;
					float _31, _32, _33, _34;
					float _41, _42, _43, _44;
				};
				float matrix[4][4];
				__m128 matrixSSE[4];
			};

			Matrix4x4() = default;
			Matrix4x4(const Matrix4x4&) = default;
			Matrix4x4& operator=(const Matrix4x4&) = default;

			Matrix4x4(Matrix4x4&&) = default;
			Matrix4x4& operator=(Matrix4x4&&) = default;
#ifdef SIMD
			// SIMD는 constexpr이 안된다?
			Matrix4x4(float m00, float m01, float m02, float m03,
				float m10, float m11, float m12, float m13,
				float m20, float m21, float m22, float m23,
				float m30, float m31, float m32, float m33)
				: _11(m00), _12(m01), _13(m02), _14(m03),
				_21(m10), _22(m11), _23(m12), _24(m13),
				_31(m20), _32(m21), _33(m22), _34(m23),
				_41(m30), _42(m31), _43(m32), _44(m33)
			{
				matrix[0][0] = m00;
				matrix[0][1] = m01;
				matrix[0][2] = m02;
				matrix[0][3] = m03;

				matrix[1][0] = m10;
				matrix[1][1] = m11;
				matrix[1][2] = m12;
				matrix[1][3] = m13;

				matrix[2][0] = m20;
				matrix[2][1] = m21;
				matrix[2][2] = m22;
				matrix[2][3] = m23;

				matrix[3][0] = m30;
				matrix[3][1] = m31;
				matrix[3][2] = m32;
				matrix[3][3] = m33;

				matrixSSE[0] = _mm_set_ps(m03, m02, m01, m00);
				matrixSSE[1] = _mm_set_ps(m13, m12, m11, m10);
				matrixSSE[2] = _mm_set_ps(m23, m22, m21, m20);
				matrixSSE[3] = _mm_set_ps(m33, m32, m31, m30);
			}
#else
			constexpr Matrix4x4(float m00, float m01, float m02, float m03,
				float m10, float m11, float m12, float m13,
				float m20, float m21, float m22, float m23,
				float m30, float m31, float m32, float m33)
				: _11(m00), _12(m01), _13(m02), _14(m03),
				_21(m10), _22(m11), _23(m12), _24(m13),
				_31(m20), _32(m21), _33(m22), _34(m23),
				_41(m30), _42(m31), _43(m32), _44(m33)
			{
				matrix[0][0] = m00;
				matrix[0][1] = m01;
				matrix[0][2] = m02;
				matrix[0][3] = m03;

				matrix[1][0] = m10;
				matrix[1][1] = m11;
				matrix[1][2] = m12;
				matrix[1][3] = m13;

				matrix[2][0] = m20;
				matrix[2][1] = m21;
				matrix[2][2] = m22;
				matrix[2][3] = m23;

				matrix[3][0] = m30;
				matrix[3][1] = m31;
				matrix[3][2] = m32;
				matrix[3][3] = m33;

			}
#endif
		};
	}
}