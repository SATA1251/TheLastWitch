#include <DirectXMath.h>
#include "MathHeaders.h"

float KunrealEngine::KunrealMath::ConvertToRadian(float degree)
{
	return degree * (3.141592654f / 180.f);
}

float KunrealEngine::KunrealMath::Create2x2Determinant(Matrix2x2 matrix)
{
	return matrix._11 * matrix._22 - matrix._12 * matrix._21;
}

KunrealEngine::KunrealMath::Matrix2x2 KunrealEngine::KunrealMath::Inverse2x2Matrix(Matrix2x2 matrix)
{
	float det = Create2x2Determinant(matrix);

	// 판별식이 0이면 역행렬은 존재하지 않는다
	if (det == 0)
	{		
		return matrix;
	}

	float invDet = 1.0f / det;

	Matrix2x2 invMatrix(matrix._22 * invDet, -matrix._12 * invDet, -matrix._21 * invDet, matrix._11 * invDet);

	return invMatrix;
}

float KunrealEngine::KunrealMath::Create3x3Determinant(Matrix3x3 matrix)
{
	return matrix._11 * (matrix._22 * matrix._33 - matrix._23 * matrix._32) 
		 - matrix._12 * (matrix._21 * matrix._33 - matrix._23 * matrix._31) 
		 + matrix._13 * (matrix._21 * matrix._32 - matrix._22 * matrix._31);
}

float KunrealEngine::KunrealMath::Create3x3Determinant(float a, float b, float c, float d, float e, float f, float g, float h, float i)
{
	return a * (e * i - f * h) - b * (d * i - f * g) + c * (d * h - e * g);
}

KunrealEngine::KunrealMath::Matrix3x3 KunrealEngine::KunrealMath::Inverse3x3Matrix(Matrix3x3 matrix)
{
	//float a = matrix._11;
	//float b = matrix._12;
	//float c = matrix._13;
	//float d = matrix._21;
	//float e = matrix._22;
	//float f = matrix._23;
	//float g = matrix._31;
	//float h = matrix._32;
	//float i = matrix._33;

	float det = Create3x3Determinant(matrix);

	// 판별식이 0이면 역행렬은 존재하지 않는다 
	if (det == 0)
	{
		return matrix;
	}

	float invDet = 1.0f / det;

	float a = (matrix._22 * matrix._33 - matrix._23 * matrix._32) * invDet;
	float b = (matrix._13 * matrix._32 - matrix._12 * matrix._33) * invDet;
	float c = (matrix._12 * matrix._23 - matrix._13 * matrix._22) * invDet;
	float d = (matrix._23 * matrix._31 - matrix._21 * matrix._33) * invDet;
	float e = (matrix._11 * matrix._33 - matrix._13 * matrix._31) * invDet;
	float f = (matrix._13 * matrix._21 - matrix._11 * matrix._23) * invDet;
	float g = (matrix._21 * matrix._32 - matrix._22 * matrix._31) * invDet;
	float h = (matrix._12 * matrix._31 - matrix._11 * matrix._32) * invDet;
	float i = (matrix._11 * matrix._22 - matrix._12 * matrix._21) * invDet;

	Matrix3x3 invMatrix(a, b, c, d, e, f, g, h, i);

	return invMatrix;
}

KunrealEngine::KunrealMath::Matrix3x3 KunrealEngine::KunrealMath::Multiply3x3Matrix(Matrix3x3 param1, Matrix3x3 param2)
{
	Matrix3x3 _result;

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			_result.matrix[i][j] = 0.f;		// 초기화

			for (int k = 0; k < 3; k++)
			{
				_result.matrix[i][j] += param1.matrix[i][k] * param2.matrix[k][j];
			}
		}
	}

	return _result;
}

float KunrealEngine::KunrealMath::Create4x4Determinant(Matrix4x4 matrix)
{
	float a = matrix._11;
	float b = matrix._12;
	float c = matrix._13;
	float d = matrix._14;
	float e = matrix._21;
	float f = matrix._22;
	float g = matrix._23;
	float h = matrix._24;
	float i = matrix._31;
	float j = matrix._32;
	float k = matrix._33;
	float l = matrix._34;
	float m = matrix._41;
	float n = matrix._42;
	float o = matrix._43;
	float p = matrix._44;

	float det1 = Create3x3Determinant(f, g, h, j, k, l, n, o, p);
	float det2 = Create3x3Determinant(e, g, h, i, k, l, m, o, p);
	float det3 = Create3x3Determinant(e, f, h, i, j, l, m, n, p);
	float det4 = Create3x3Determinant(e, f, g, i, j, k, m, n, o);

	return (a * det1) - (b * det2) + (c * det3) - (d * det4);
}

KunrealEngine::KunrealMath::Matrix4x4 KunrealEngine::KunrealMath::Inverse4x4Matrix(Matrix4x4 matrix)
{
	float a = matrix._11;
	float b = matrix._12;
	float c = matrix._13;
	float d = matrix._14;
	float e = matrix._21;
	float f = matrix._22;
	float g = matrix._23;
	float h = matrix._24;
	float i = matrix._31;
	float j = matrix._32;
	float k = matrix._33;
	float l = matrix._34;
	float m = matrix._41;
	float n = matrix._42;
	float o = matrix._43;
	float p = matrix._44;

	float det = Create4x4Determinant(matrix);

	if (det == 0)
	{
		return matrix;
	}

	float invDet = 1.0f / det;

	float m1 = invDet * Create3x3Determinant(f, g, h, j, k, l, n, o, p);
	float m2 = - invDet * Create3x3Determinant(b, c, d, j, k, l, n, o, p);
	float m3 = invDet * Create3x3Determinant(b, c, d, f, g, h, n, o, p);
	float m4 = - invDet * Create3x3Determinant(b, c, d, f, g, h, j, k, l);
	float m5 = - invDet * Create3x3Determinant(e, g, h, i, k, l, m, o, p);
	float m6 = invDet * Create3x3Determinant(a, c, d, i, k, l, m, o, p);
	float m7 = - invDet * Create3x3Determinant(a, c, d, e, g, h, m, o, p);
	float m8 = invDet * Create3x3Determinant(a, c, d, e, g, h, i, k, l);
	float m9 = invDet * Create3x3Determinant(e, f, h, i, j, l, m, n, p);
	float m10 = - invDet * Create3x3Determinant(a, b, d, i, j, l, m, n, p);
	float m11 = invDet * Create3x3Determinant(a, b, d, e, f, h, m, n, p);
	float m12 = - invDet * Create3x3Determinant(a, b, d, e, f, h, i, j, l);
	float m13 = - invDet * Create3x3Determinant(e, f, g, i, j, k, m, n, o);
	float m14 = invDet * Create3x3Determinant(a, b, c, i, j, k, m, n, o);
	float m15 = - invDet * Create3x3Determinant(a, b, c, e, f, g, m, n, o);
	float m16 = invDet * Create3x3Determinant(a, b, c, e, f, g, i, j, k);

	Matrix4x4 invMatrix(m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13, m14, m15, m16);

	return invMatrix;
}

KunrealEngine::KunrealMath::Matrix4x4 KunrealEngine::KunrealMath::Multiply4x4Matrix(Matrix4x4 param1, Matrix4x4 param2)
{
	Matrix4x4 _result;

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			_result.matrix[i][j] = 0.f;		// 초기화

			for (int k = 0; k < 4; k++)
			{
				_result.matrix[i][j] += param1.matrix[i][k] * param2.matrix[k][j];
			}
		}
	}

	return _result;
}

KunrealEngine::KunrealMath::Matrix4x4 KunrealEngine::KunrealMath::Multiply4x4Matrix(Matrix4x4 param1, Matrix4x4 param2, Matrix4x4 param3)
{
	Matrix4x4 _result;

	_result = Multiply4x4Matrix(Multiply4x4Matrix(param1, param2), param3);

	return _result;
}

KunrealEngine::KunrealMath::Matrix4x4 KunrealEngine::KunrealMath::Transpose4x4Matrix(Matrix4x4 matrix)
{
	return Matrix4x4(matrix._11, matrix._21, matrix._31, matrix._41, 
					matrix._12, matrix._22, matrix._32, matrix._42, 
					matrix._13, matrix._23, matrix._33, matrix._43, 
					matrix._14, matrix._24, matrix._34, matrix._44);
}

KunrealEngine::KunrealMath::Matrix4x4 KunrealEngine::KunrealMath::CreateTranslateMatrix(Float3 pos)
{
	Matrix4x4 trans(1.f, 0.f, 0.f, 0.f,
					0.f, 1.f, 0.f, 0.f,
					0.f, 0.f, 1.f, 0.f,
					pos.x, pos.y, pos.z, 1.f);

	return trans;
}

KunrealEngine::KunrealMath::Matrix4x4 KunrealEngine::KunrealMath::CreateScaleMatrix(Float3 scale)
{
	Matrix4x4 scaleMatrix(scale.x, 0.f, 0.f, 0.f,
						  0.f, scale.y, 0.f, 0.f,
						  0.f, 0.f, scale.z, 0.f,
						  0.f, 0.f, 0.f, 1.f);

	return scaleMatrix;
}

KunrealEngine::KunrealMath::Matrix4x4 KunrealEngine::KunrealMath::CreateQuaternionMatrix(Float3 rotation)
{
	//static const Float4 sign(1.f, -1.f, -1.f, 1.f);
	//static const Float4 half(0.5f, 0.5f, 0.5f, 0.5f);
	//
	//Float4 angles(ConvertToRadian(rotation.x), ConvertToRadian(rotation.y), ConvertToRadian(rotation.z), 0.0f);	
	//
	//Float4 halfAngles(angles * half);
	//Float4 sinAngles(sinf(halfAngles.x), sinf(halfAngles.y), sinf(halfAngles.z), sinf(halfAngles.w));
	//Float4 cosAngles(cosf(halfAngles.x), cosf(halfAngles.y), cosf(halfAngles.z), cosf(halfAngles.w));

	//Float3 angles(ConvertToRadian(rotation.x), ConvertToRadian(rotation.y), ConvertToRadian(rotation.z));
	//
	//float sinX = sinf(angles.x);
	//float cosX = cosf(angles.x);
	//
	//float sinY = sinf(angles.y);
	//float cosY = cosf(angles.y);
	//
	//float sinZ = sinf(angles.z);
	//float cosZ = cosf(angles.z);
	//
	///// X Y Z 순서
	//
	//Matrix4x4 정용훈(cosY * cosZ,						-cosY * sinZ,						sinY,			0.f,
	//				sinX * sinY * cosZ + cosX * sinZ,	-sinX * sinY * sinZ + cosX * cosZ, -sinX * cosY,	0.f,
	//				-cosX * sinY * cosZ + sinX * sinZ,	cosX * sinY * sinZ + sinX * cosZ,	cosX * cosY,	0.f,
	//				0.f,								0.f,								0.f,			1.f);
	//
	///// Z X Y 순서
	//
	//	Matrix4x4 matrix(
	//		-sinZ * sinX * sinY + cosZ * cosY,		-sinZ * cosX,		sinZ * sinX * cosY + cosZ * sinY ,		0.f,
	//		cosZ * sinX * sinY + sinZ * cosY,		cosZ * cosX,		-cosZ * sinX * cosY + sinZ * sinY,		0.f,
	//		-cosX * sinY,							sinX,				cosX * cosY,							0.f,
	//		0.f,									0.f,				0.f,									1.f
	//	);

	//matrix = Transpose4x4Matrix(matrix);

	/// 진도가 너무 안나가서 DirectXMath 함수를 사용해서 쿼터니언 변환
	/// 추후 시간내서 완성해보자

	DirectX::XMFLOAT4 quat;
	DirectX::XMVECTOR rollPitchYaw = DirectX::XMQuaternionRotationRollPitchYaw(ConvertToRadian(rotation.x), ConvertToRadian(rotation.y), ConvertToRadian(rotation.z));

	DirectX::XMStoreFloat4(&quat, rollPitchYaw);
	DirectX::XMMATRIX rotationXMMatrix = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&quat));
	DirectX::XMFLOAT4X4 rotationMatrix; 
	XMStoreFloat4x4(&rotationMatrix, rotationXMMatrix);

	Matrix4x4 quaternionMatrix(rotationMatrix._11, rotationMatrix._12, rotationMatrix._13, rotationMatrix._14,
							   rotationMatrix._21, rotationMatrix._22, rotationMatrix._23, rotationMatrix._24,
							   rotationMatrix._31, rotationMatrix._32, rotationMatrix._33, rotationMatrix._34,
							   rotationMatrix._41, rotationMatrix._42, rotationMatrix._43, rotationMatrix._44
	);
	
	return quaternionMatrix;
	
}
