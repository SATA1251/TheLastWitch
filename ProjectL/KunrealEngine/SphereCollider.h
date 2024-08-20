#pragma once

#include <DirectXMath.h>
#include "CommonHeader.h"
#include "Component.h"

namespace physx
{
	class PxShape;
}

namespace KunrealEngine
{
	class Transform;

	class _DECLSPEC SphereCollider : public Component
	{
		friend class PhysicsSystem;
		friend class GameObject;
	private:
		SphereCollider();
	public:
		~SphereCollider();

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
		Transform* _transform;

		// 움직이는가	// default는 false->움직이는 물체
		bool _isStatic;

		// collider 반지름
		float _radius;

		// collider 높이		// 중점에서부터의 높이임 실제 크기는 x2
		float _halfHeight;

		// physx에 넘겨줄 Transform
		DirectX::XMFLOAT3 _position;

		// 오브젝트의 Transform으로부터 얼마나 차이가 날 것인지
		DirectX::XMFLOAT3 _offset;

		// 충돌했는지
		bool _isCollided;

		// 어떤 오브젝트와 충돌했는지
		GameObject* _targetObj;

		// physx의 collider의 shape 변수
		physx::PxShape* _shape;

	public:
		// 충돌여부 반환
		bool IsCollided();

		// 충돌한 대상의 오브젝트 반환
		GameObject* GetTargetObject();

		// 오브젝트의 Transform으로부터 얼마나 차이가 날 것인지 설정
		void SetOffset(float x, float y, float z);

		// offset 수치를 반환	// default는 0 0 0
		DirectX::XMFLOAT3 GetOffset();

		// 반지름 설정
		void SetRadius(float radius);

		// 높이 설정		// 중점에서부터의 높이임 실제 크기는 x2
		void SetHalfHeight(float height);

		// 반지름 반환
		float GetRadius();

		// 높이 반환
		float GetHalfHeight();


	};
}


