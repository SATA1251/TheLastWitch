#pragma once

#include "Collider.h"
#include "Component.h"
#include "GraphicsSystem.h"

namespace KunrealEngine
{
	class _DECLSPEC BoxCollider : public Component, public Collider
	{
		friend class PhysicsSystem;
		friend class GameObject;

		friend class Player;		/// ���߿� ����
	private:
		BoxCollider();
	public:
		~BoxCollider();

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
		// wireframe���� collider�� ũ�⸦ ǥ������
		GInterface::GraphicsDebug* _debugObject;

	public:
		// Debug Object ����
		void SetDebugMeshData();

		// collider ũ�⸦ ������
		virtual void SetColliderScale(float x, float y, float z) override;
		virtual void SetColliderScale(const DirectX::XMFLOAT3& scale) override;

		/// Mesh Render���� �ݶ��̴��� Ư�� ���� ���̱� ����
	public:
		// �θ�, �θ��� ���� �����ϸ� �� ���� �θ�� �ΰ� ���� �ٴϰԵ�
		void SetTransform(GameObject* renderable, std::string boneName);
		// �θ� �ִٸ� �и��Ͽ� �θ� ���� �ʵ��� ��
		void SeperateFromParent();

	private:
		void CalculateParentBone();

	private:
		GameObject* _parentObject;
		std::string _parentBoneName;
	};
}


