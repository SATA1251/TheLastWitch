#pragma once

#include <DirectXMath.h>
#include "CommonHeader.h"
#include "Component.h"

namespace GInterface
{
	class GraphicsParticle;
}

namespace KunrealEngine
{
	class Transform;

	class _DECLSPEC Particle : public Component
	{
		friend class GameObject;
	public:
		Particle();
		~Particle();

		void Initialize() override;
		void Release() override;

		void FixedUpdate() override;
		void Update() override;
		void LateUpdate() override;

		void OnTriggerEnter() override;
		void OnTriggerStay() override;
		void OnTriggerExit() override;

		void SetActive(bool active) override;

	private:
		// 그래픽스의 파티클 객체
		GInterface::GraphicsParticle* _particle;

		// 파티클을 담을 오브젝트의 transform
		Transform* _transform;

		DirectX::XMFLOAT2 _size;		// 파티클 크기
		float _velocity;				// 방출 강도
		bool _random;					// 랜덤으로 흩뿌리는지
		float _fadeoutTime;				// 서서히 흐려지게하는 시간
		float _lifeTime;				// 얼마 뒤에 사라지게 할건가
		DirectX::XMFLOAT3 _color;		// RGB		// 이 색을 추가하겠다
		DirectX::XMFLOAT3 _direction;	// 방출 방향
		DirectX::XMFLOAT3 _rotation;	// 회전 각도
		DirectX::XMFLOAT3 _offset;		// 위치 여백

		DirectX::XMFLOAT3 _decomposedPos;
		DirectX::XMFLOAT3 _decomposedRot;
		DirectX::XMFLOAT3 _decomposedScale;

	public:
		// 어떤 파티클을 출력할 것인가		// 처음에 반드시 해줘야함
		void SetParticleEffect(std::string name, std::string fileName, unsigned int maxParticle);

		// 파티클 방출 혹은 분사 
		void Start();

		// 파티클 방출/분사를 중단
		void Stop();

		// 파티클을 처음 위치부터 다시 분사
		void Reset();

		// 파티클 위치
		void SetParticlePos(float x, float y, float z);
		void SetParticlePos(DirectX::XMFLOAT3 pos);

		// 파티클 크기 설정
		void SetParticleSize(float x, float y);

		// 파티클 크기 반환
		DirectX::XMFLOAT2 GetParticleSize();

		// 파티클 방출 강도 및 방출각도를 무작위로 할것인지 여부
		void SetParticleVelocity(float velocity, bool isRandom);

		// 파티클이 fadeout 되는 시간 및 완전히 사라지는 시간 설정	// 두 개가 같을 필요 없음
		void SetParticleDuration(float fade, float life);

		// 파티클에 색을 입히는 함수	// 기본적으로 색을 가지고 있음
		// 0.0 ~ 1.0
		void AddParticleColor(float x, float y, float z);

		// 파티클 방출 방향 설정		// 수치가 높을 수록 해당 방향으로 가속도가 붙음
		/// 지금 안쓰인데 쓰지마 나중에 수정
		void SetParticleDirection(float x, float y, float z);

		// 파티클 rotation 설정
		void SetParticleRotation(float x, float y, float z);

		// 특정 본으로 트랜스폼 설정
		void SetTransform(GameObject* renderable, std::string boneName);

		// offset 설정
		void SetOffSet(float x, float y, float z);

		// 평면 장판 위해 각도 설정
		void SetParticleAngle(DirectX::XMFLOAT3& angle);
		void SetParticleAngle(float x, float y, float z);

		// 평면 장판 위해 각도 설정
		void SetParticleAngle(float angle);
		DirectX::XMFLOAT3& GetParticleAngle();

		void SetParticleCameraApply(bool tf);
		bool GetParticleCameraApply();

	public:
		// 파티클 정보 전달용
		DirectX::XMFLOAT2 GetSize();
		float GetVelocity();
		bool GetRandomState();
		float GetFadeOutTime();
		float GetLifeTime();
		DirectX::XMFLOAT3 GetColor();
		DirectX::XMFLOAT3 GetDirection();
		DirectX::XMFLOAT3 GetRotation();

	private:
		GameObject* _parentObject;
		std::string _parentBoneName;
	};
}

