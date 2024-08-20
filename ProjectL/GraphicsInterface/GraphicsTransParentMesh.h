/// <summary>
/// 투명화할 오브젝트를 위한 클래스
/// 
/// 윤종화
/// </summary>

#pragma once
#include <string>
#include <DirectXMath.h>

namespace GInterface
{
	class GraphicsTransparentMesh
	{
	public:
		virtual ~GraphicsTransparentMesh() {};

	public:
		virtual void Delete() abstract;

		// 메쉬의 텍스쳐 설정
		virtual void SetTexture(const std::string& textureName) abstract;

		// timer의 시간만큼 그리기 (루프에 넣어야함)
		virtual bool RenderWithTimer(float deltaTime, float timer) abstract;
 
		// timer 초기화
		virtual void Reset() abstract;

		// 0번 : 기본
		// 1번 : 좌->우로 그림 
		// 2번 : 아래 -> 위로 그림 
		// 3번 중앙 -> 외곽으로 그림
		// 4번 중앙 -> 외곽선은 남긴채로 채워줌 (circle일때만 제대로 동작)
		// 5번 도넛 -> 중간에 특정 공간만 원형으로 남긴채 채움
		virtual void SetRenderType(unsigned int index) abstract;

		// 포지션 설정
		virtual void SetTransform(const DirectX::XMFLOAT4X4& matrix) abstract;

		// 도넛을 위한 레인지 설정
		virtual void SetExceptRange(const DirectX::XMFLOAT3& center, float range) abstract;

		virtual void SetStartFlag(bool tf) abstract;

		virtual void SetDecal(bool tf) abstract;

		virtual void SetInfinite(bool tf) abstract;

		virtual bool GetInfiniteState() abstract;

		virtual bool GetIsApplyPattern() abstract;

		virtual void ApplyPattern(bool tf) abstract;

		virtual float GetRenderTimer() abstract;
	}; 
}