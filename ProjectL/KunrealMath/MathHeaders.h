#pragma once

#include "Float2.h"
#include "Float3.h"
#include "Float4.h"
#include "Matrix2x2.h"
#include "Matrix3x3.h"
#include "Matrix4x4.h"

namespace KunrealEngine
{
	namespace KunrealMath
	{
		// ����
		float ConvertToRadian(float degree);

		// 2x2 ����� �Ǻ���
		float Create2x2Determinant(Matrix2x2 matrix);

		// 2x2 ����� �����
		Matrix2x2 Inverse2x2Matrix(Matrix2x2 matrix);

		// 3x3 ����� �Ǻ���
		float Create3x3Determinant(float a, float b, float c, float d, float e, float f, float g, float h, float i);
		float Create3x3Determinant(Matrix3x3 matrix);

		// 3x3 ����� �����
		Matrix3x3 Inverse3x3Matrix(Matrix3x3 matrix);

		// 3x3 ��� ����
		Matrix3x3 Multiply3x3Matrix(Matrix3x3 param1, Matrix3x3 param2);

		// 4x4 ����� �Ǻ���
		float Create4x4Determinant(Matrix4x4 matrix);

		// 4x4 ����� �����
		Matrix4x4 Inverse4x4Matrix(Matrix4x4 matrix);

		// 4x4 ����� ����
		Matrix4x4 Multiply4x4Matrix(Matrix4x4 param1, Matrix4x4 param2);
		Matrix4x4 Multiply4x4Matrix(Matrix4x4 param1, Matrix4x4 param2, Matrix4x4 param3);

		// 4x4 ����� ��ġ
		Matrix4x4 Transpose4x4Matrix(Matrix4x4 matrix);

		// Position ���ȭ
		Matrix4x4 CreateTranslateMatrix(Float3 pos);

		// Scale ���ȭ
		Matrix4x4 CreateScaleMatrix(Float3 scale);

		// Rotation ���ȭ
		Matrix4x4 CreateQuaternionMatrix(Float3 rotation);
	}
}