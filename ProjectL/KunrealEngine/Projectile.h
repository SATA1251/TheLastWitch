#pragma once
#include <DirectXMath.h>
#include <functional>
#include "CommonHeader.h"
#include "Component.h"

/// <summary>
/// 플레이어가 발사할 투사체 클래스
/// 
/// 생성될 때 Collider, MeshRenderer 도 함께 추가됌
/// </summary>

namespace KunrealEngine
{
	class GameObject;
	class BoxCollider;
	class MeshRenderer;

	class _DECLSPEC Projectile : public Component
	{
		friend class PlayerAbility;
	public:
		Projectile();
		~Projectile();

	public:
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
		BoxCollider* _collider;				// 충돌 감지해주는 컴포넌트
		MeshRenderer* _mesh;				// 투사체를 그려줄 3D 물체

		DirectX::XMVECTOR _direction;		// 플레이어가 바라보는 방향벡터
		std::function<bool()> _condition;	// 투사체 소멸 조건

	public:
		float _movedRange;					// 투사체가 이동한 거리 계산
		// 투사체가 발사될 방향
		void SetDirection(DirectX::XMVECTOR direction);

		// 어떤 물체를 그릴 지	// 이펙트는 쉐이더를 통해
		void SetMeshObject(const char* meshName, const char* textureName = "", const char* normalName = "");

		// 콜라이더 객체 반환
		BoxCollider* GetCollider();
			
		// 방향벡터 반환 (플레이어의 방향벡터)
		DirectX::XMVECTOR GetDirection();

		// 소멸 조건 설정
		void SetDestoryCondition(std::function<bool()> cond);

		// 로직 처리 후 조건 false로 초기화
		void ResetCondition();
	};
}

