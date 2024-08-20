#pragma once
#include "CommonHeader.h"
#include <DirectXMath.h>

/// <summary>
/// ���ӷ����� �����ϴ� Ŭ����
/// �Լ��� ��� static
/// </summary>

namespace KunrealEngine
{
	class ToolBox
	{
		/// ���� ����
	public:
		// 0���� �Ű������� ������ ����
		static int GetRandomNum(int maxNum);

		// ���� ����
		static int GetRandomNum(int minNum, int maxNum);

		// float �� random ��ȯ
		static float GetRandomFloat(float minNum, float maxNum);

		/// ���� ����
	public:
		// �� XMFLOAT3 ��ǥ ������ �Ÿ�
		static float GetDistance(DirectX::XMFLOAT3& src, DirectX::XMFLOAT3& dst);

		// �� XMFLOAT3 ��ǥ ������ ���⺤��
		static DirectX::XMVECTOR GetDirectionVec(DirectX::XMFLOAT3& src, DirectX::XMFLOAT3& dst);

		// �� XMFLOAT3 ��ǥ ���հ�
		static float GetAngle(DirectX::XMFLOAT3 src, DirectX::XMFLOAT3 dst);

		static float GetAngleWithDirection(const DirectX::XMFLOAT3& src, const DirectX::XMFLOAT3& dst, float prevAngle);

		// ���ʹϾ��� ���Ϸ���
		static DirectX::XMFLOAT3 QuaternionToEulerAngles(const DirectX::XMVECTOR& quaternion);

		// ���� ���� ������ŭ ȸ����Ű��
		static DirectX::XMFLOAT3 RotateVector(const DirectX::XMFLOAT3& direction, float angle);

		// ���� ���� ������ŭ ȸ����Ű��
		static DirectX::XMFLOAT3 RotateVector(const DirectX::XMFLOAT3& direction, DirectX::XMVECTOR& quaternion);

		// �� ���� ������ ���̰� ����
		static float CalculateAngleBetweenVectors(const DirectX::XMFLOAT3& vec1, const DirectX::XMFLOAT3& vec2);
	
		/// �ݺ����� �������� 1���� ȣ��ǵ��� �ؾ���
		// �������� ��ǥ�� vector<XMFLOAT3> ������ �־��ִ� �Լ�
											// �����						// ������					// ���ӽð�		// �߷°��ӵ�		// �־��� ��� ����
		static void CalculateParabolaPath(const DirectX::XMFLOAT3& src, const DirectX::XMFLOAT3& dst, float duration, float gravity, std::vector<DirectX::XMFLOAT3>& path);

		// �� ��ǥ�� ���������ϴ� �Լ�
		static DirectX::XMFLOAT3 Lerp(DirectX::XMFLOAT3 p0, DirectX::XMFLOAT3 p1, float t);

		// XMFLOAT3 �E��
		static DirectX::XMFLOAT3 XMFLOAT3Subtract(const DirectX::XMFLOAT3& a, const DirectX::XMFLOAT3& b);
		// XMFLOAT3 ����
		static DirectX::XMFLOAT3 XMFLOAT3Add(const DirectX::XMFLOAT3& a, const DirectX::XMFLOAT3& b);
	
		// ������ ����
		static DirectX::XMFLOAT3 Bezier(DirectX::XMFLOAT3 startPoint, DirectX::XMFLOAT3 p1, DirectX::XMFLOAT3 p2, DirectX::XMFLOAT3 endPoint, float t);

		static DirectX::XMFLOAT3 LogarithmicInterpolation(const DirectX::XMFLOAT3& startPos, const DirectX::XMFLOAT3& endPos, float t);
	};

}
