#pragma once
#include <DirectXMath.h>
#include <functional>
#include "CommonHeader.h"
#include "Component.h"

/// <summary>
/// �÷��̾ �߻��� ����ü Ŭ����
/// 
/// ������ �� Collider, MeshRenderer �� �Բ� �߰���
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
		BoxCollider* _collider;				// �浹 �������ִ� ������Ʈ
		MeshRenderer* _mesh;				// ����ü�� �׷��� 3D ��ü

		DirectX::XMVECTOR _direction;		// �÷��̾ �ٶ󺸴� ���⺤��
		std::function<bool()> _condition;	// ����ü �Ҹ� ����

	public:
		float _movedRange;					// ����ü�� �̵��� �Ÿ� ���
		// ����ü�� �߻�� ����
		void SetDirection(DirectX::XMVECTOR direction);

		// � ��ü�� �׸� ��	// ����Ʈ�� ���̴��� ����
		void SetMeshObject(const char* meshName, const char* textureName = "", const char* normalName = "");

		// �ݶ��̴� ��ü ��ȯ
		BoxCollider* GetCollider();
			
		// ���⺤�� ��ȯ (�÷��̾��� ���⺤��)
		DirectX::XMVECTOR GetDirection();

		// �Ҹ� ���� ����
		void SetDestoryCondition(std::function<bool()> cond);

		// ���� ó�� �� ���� false�� �ʱ�ȭ
		void ResetCondition();
	};
}

