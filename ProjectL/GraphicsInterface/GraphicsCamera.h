/// <summary>
/// Camera 객체에서 
/// 게임 엔진쪽에서 사용할 함수를 모아놓은 인터페이스 클래스
/// 
/// 윤종화
/// </summary>

#pragma once

namespace GInterface
{
	class GraphicsCamera
	{
	public:
		virtual ~GraphicsCamera() {};

	public:
		// 카메라 객체 삭제
		virtual void Delete() abstract;

	public:
		virtual DirectX::XMFLOAT3 GetCameraPosition() abstract;
		// 카메라 위치 설정
		virtual void SetCameraPosition(const DirectX::XMFLOAT3& position) abstract;
		// 카메라가 바라보는 타겟 위치 설정
		virtual void SetTargetPosition(const DirectX::XMFLOAT3& position) abstract;
		// 카메라 회전 angle.x (위아래), angle.y (좌우)
		virtual void RotateCamera(const DirectX::XMFLOAT2& angle) abstract;

	public:
		// 카메라가 바라보는 방향에서 오른쪽을 향하는 벡터 가져오기
		virtual const DirectX::XMFLOAT3 GetRightVector() abstract;
		// 카메라가 바라보는 방향 벡터 가져오기
		virtual const DirectX::XMFLOAT3 GetLookVector() abstract;
		// 카메라가 바라보는 방향에서 위를 향하는 백터 가져오기
		virtual const DirectX::XMFLOAT3 GetUpVector() abstract;

	};
}