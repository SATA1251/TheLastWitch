#pragma once
#include <string>

namespace GInterface
{
	class GraphicsParticle
	{
	public:
		GraphicsParticle() {};
		virtual ~GraphicsParticle() {};

	public:
		// 파티클 방출
		virtual void Start() abstract;
		// 파티클 방출 멈춤
		virtual void Stop() abstract;

		// 파티클이 실행중인지 여부 반환
		virtual bool GetParticleState() abstract;

	public:
		// 파티클의 위치 설정
		virtual void SetEmitPos(const DirectX::XMFLOAT3& emitPos) abstract;
		// 파티클의 크기 설정
		virtual void SetParticleSize(const DirectX::XMFLOAT2& particleSize) abstract;
		// 파티클의 방출 쌔기와 랜덤방출 설정
		virtual void SetEmitVelocity(float emitVelocity, bool isRandom) abstract;
		// 파티클의 방출 후 점점색이 바래지는 시간(Fade)과 완전소멸시간(Life) 설정
		virtual void SetParticleTime(float particleFadeTime, float particleLifeTime) abstract;
		// 파티클의 색상 설정(RGB)
		virtual void SetParticleColor(const DirectX::XMFLOAT3& particleColor) abstract;
		// 파티클의 방출 방향 설정
		virtual void SetParticleDirection(const DirectX::XMFLOAT3& particleDirection) abstract;
		// 파티클의 회전 설정
		virtual void SetParticleRotation(const DirectX::XMFLOAT3& particleRotation) abstract;

		// 파티클 각도에 따라 로테이션 설정 (평면 장판 위함)
		virtual void SetParticleAngle(DirectX::XMFLOAT3& angle) abstract;

		// 파티클 각도에 따라 로테이션 반환
		virtual DirectX::XMFLOAT3& GetParticleAngle() abstract;

		// 파티클 전용 카메라를 써서 렌더링 할지 여부 (평면 장판 위함)
		virtual void SetParticleCameraApply(bool tf) abstract;

		// 파티클 전용 카메라를 써서 렌더링 할지 여부 (평면 장판 위함)
		virtual bool GetParticleCameraApply() abstract;

		// 파티클을 다시 0부터 시작하게 만든다
		virtual void Reset() abstract;

		// 포워드 렌더링으로 렌더링할지 여부 설정
		virtual void SetForwardRendering(bool tf) abstract;

		// 포워드 렌더링으로 렌더링할지 여부 가져오기
		virtual bool GetIsForwardRendering() abstract;
	};
}