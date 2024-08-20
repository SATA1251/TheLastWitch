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

		// �����̴°�	// default�� false->�����̴� ��ü
		bool _isStatic;

		// collider ������
		float _radius;

		// collider ����		// �������������� ������ ���� ũ��� x2
		float _halfHeight;

		// physx�� �Ѱ��� Transform
		DirectX::XMFLOAT3 _position;

		// ������Ʈ�� Transform���κ��� �󸶳� ���̰� �� ������
		DirectX::XMFLOAT3 _offset;

		// �浹�ߴ���
		bool _isCollided;

		// � ������Ʈ�� �浹�ߴ���
		GameObject* _targetObj;

		// physx�� collider�� shape ����
		physx::PxShape* _shape;

	public:
		// �浹���� ��ȯ
		bool IsCollided();

		// �浹�� ����� ������Ʈ ��ȯ
		GameObject* GetTargetObject();

		// ������Ʈ�� Transform���κ��� �󸶳� ���̰� �� ������ ����
		void SetOffset(float x, float y, float z);

		// offset ��ġ�� ��ȯ	// default�� 0 0 0
		DirectX::XMFLOAT3 GetOffset();

		// ������ ����
		void SetRadius(float radius);

		// ���� ����		// �������������� ������ ���� ũ��� x2
		void SetHalfHeight(float height);

		// ������ ��ȯ
		float GetRadius();

		// ���� ��ȯ
		float GetHalfHeight();


	};
}


