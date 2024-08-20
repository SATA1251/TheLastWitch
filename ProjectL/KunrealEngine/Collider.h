#pragma once
#include <DirectXMath.h>
#include "CommonHeader.h"

namespace physx
{
	class PxShape;
}

namespace KunrealEngine 
{
	class GameObject;
	class Transform;

	class _DECLSPEC Collider
	{
		friend class PhysicsSystem;
	public:
		Collider();
		virtual ~Collider();

	private:

	protected:
		bool _isCollided;				// 충돌했는지
		bool _colliderActivated;		// collider 활성화 정보
		Transform* _transform;			// collider의 transform 정보
		GameObject* _ownerObj;			// collider 컴포넌트를 가지고 있는 오브젝트
		physx::PxShape* _shape;			// physx에서 가지고 있는 collider의 shape
		GameObject* _targetObj;			// 어떤 오브젝트와 충돌했는지
		
		DirectX::XMFLOAT3 _position;	// PhysX에 넘겨줄 위치값
		DirectX::XMFLOAT3 _scale;		// physx에 넘겨줄 크기	// transform과 별개임
		DirectX::XMFLOAT4 _quaternion;	// physX에 넘겨줄 Quaternion
		DirectX::XMFLOAT3 _offset;		// 오브젝트의 Transform으로부터 얼마나 차이가 날 것인지

	public:
		// 충돌 여부 반환
		bool IsCollided();

		// collider 컴포넌트의 오브젝트 반환
		GameObject* GetOwnerObject();

		// 충돌한 대상 오브젝트 반환
		GameObject* GetTargetObject();

		// 오브젝트의 Transform으로부터 얼마나 차이가 날 것인지 설정
		void SetOffset(float x, float y, float z);
		void SetOffset(const DirectX::XMFLOAT3& offset);

		// offset 수치를 반환		// 기본값은 0 0 0
		DirectX::XMFLOAT3 GetOffset();

		// collider의 위치값 반환
		DirectX::XMFLOAT3 GetColliderPos();

		// collider의 크기 반환
		DirectX::XMFLOAT3 GetColliderScale();

		// collider의 쿼터니언 반환
		DirectX::XMFLOAT4 GetColliderQuaternion();

		// 충돌여부를 false로, 대상 object를 nullptr로
		void Clear();

		// physx actor의 활동 여부 조절
		void SetActorState(bool active);

	public:
		/// collider 종류별로 다르게 동작해야하는 가상함수들
		virtual void SetColliderScale(float x, float y, float z) abstract;
		virtual void SetColliderScale(const DirectX::XMFLOAT3& scale) abstract;
	};
}

