/// <summary>
/// 콜라이더 위치 표시를 위한 디버그 객체 인터페이스 클래스
/// 
/// 윤종화
/// </summary>

#pragma once

namespace GInterface
{
	class GraphicsDebug
	{
	public:
		virtual ~GraphicsDebug() {};

	public:
		// 디버그 객체 그리기 여부 가져오기
		virtual bool GetActive() abstract;
		// 디버그 객체 그리기 여부 설정
		virtual void SetActive(bool tf) abstract;

		// 디버그 객체의 트랜스폼 설정	
		virtual void SetTransform(const DirectX::XMFLOAT4X4& matrix) abstract;
		// 디버그 객체의 위치값 설정
		virtual void SetPosition(float x = 0.0f, float y = 0.0f, float z = 0.0f) abstract;
		// 디버그 객체의 회전값 설정
		virtual void SetRotation(float x = 0.0f, float y = 0.0f, float z = 0.0f) abstract;
		// 디버그 객체의 크기값 설정
		virtual void SetScale(float x = 1.0f, float y = 1.0f, float z = 1.0f) abstract;

		// 디버그 객체 삭제
		virtual void Delete() abstract;
	};
}