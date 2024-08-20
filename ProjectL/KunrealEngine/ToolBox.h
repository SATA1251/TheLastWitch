#pragma once
#include "CommonHeader.h"
#include <DirectXMath.h>

/// <summary>
/// 게임로직을 보조하는 클래스
/// 함수는 모두 static
/// </summary>

namespace KunrealEngine
{
	class ToolBox
	{
		/// 랜덤 관련
	public:
		// 0부터 매개변수의 수까지 랜덤
		static int GetRandomNum(int maxNum);

		// 범위 랜덤
		static int GetRandomNum(int minNum, int maxNum);

		// float 값 random 반환
		static float GetRandomFloat(float minNum, float maxNum);

		/// 수학 관련
	public:
		// 두 XMFLOAT3 좌표 사이의 거리
		static float GetDistance(DirectX::XMFLOAT3& src, DirectX::XMFLOAT3& dst);

		// 두 XMFLOAT3 좌표 사이의 방향벡터
		static DirectX::XMVECTOR GetDirectionVec(DirectX::XMFLOAT3& src, DirectX::XMFLOAT3& dst);

		// 두 XMFLOAT3 좌표 사잇각
		static float GetAngle(DirectX::XMFLOAT3 src, DirectX::XMFLOAT3 dst);

		static float GetAngleWithDirection(const DirectX::XMFLOAT3& src, const DirectX::XMFLOAT3& dst, float prevAngle);

		// 쿼터니언을 오일러로
		static DirectX::XMFLOAT3 QuaternionToEulerAngles(const DirectX::XMVECTOR& quaternion);

		// 백터 일정 각도만큼 회전시키기
		static DirectX::XMFLOAT3 RotateVector(const DirectX::XMFLOAT3& direction, float angle);

		// 백터 일정 각도만큼 회전시키기
		static DirectX::XMFLOAT3 RotateVector(const DirectX::XMFLOAT3& direction, DirectX::XMVECTOR& quaternion);

		// 두 백터 사이의 사이각 리턴
		static float CalculateAngleBetweenVectors(const DirectX::XMFLOAT3& vec1, const DirectX::XMFLOAT3& vec2);
	
		/// 반복문에 넣지말고 1번만 호출되도록 해야함
		// 포물선의 좌표를 vector<XMFLOAT3> 변수에 넣어주는 함수
											// 출발점						// 도착점					// 지속시간		// 중력가속도		// 넣어줄 경로 변수
		static void CalculateParabolaPath(const DirectX::XMFLOAT3& src, const DirectX::XMFLOAT3& dst, float duration, float gravity, std::vector<DirectX::XMFLOAT3>& path);

		// 두 좌표를 선형보간하는 함수
		static DirectX::XMFLOAT3 Lerp(DirectX::XMFLOAT3 p0, DirectX::XMFLOAT3 p1, float t);

		// XMFLOAT3 뺼샘
		static DirectX::XMFLOAT3 XMFLOAT3Subtract(const DirectX::XMFLOAT3& a, const DirectX::XMFLOAT3& b);
		// XMFLOAT3 덧샘
		static DirectX::XMFLOAT3 XMFLOAT3Add(const DirectX::XMFLOAT3& a, const DirectX::XMFLOAT3& b);
	
		// 베지어 공식
		static DirectX::XMFLOAT3 Bezier(DirectX::XMFLOAT3 startPoint, DirectX::XMFLOAT3 p1, DirectX::XMFLOAT3 p2, DirectX::XMFLOAT3 endPoint, float t);

		static DirectX::XMFLOAT3 LogarithmicInterpolation(const DirectX::XMFLOAT3& startPos, const DirectX::XMFLOAT3& endPos, float t);
	};

}
