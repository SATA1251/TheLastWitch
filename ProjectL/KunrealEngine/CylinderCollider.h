#pragma once
#include "CommonHeader.h"
#include "Component.h"
#include "Collider.h"
#include "GraphicsSystem.h"

/// <summary>
/// ����� ����� debugMesh�� ������ ���� ����� �ʿ� ����δ�
/// 
/// �θ� bone�� ���̴� ����� ���� �ʿ��ϸ� ����ȣ���� ��û
/// </summary>

namespace KunrealEngine
{
	class _DECLSPEC CylinderCollider : public Component, public Collider 
	{
		friend class GameObject;
	private:
		CylinderCollider();

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

	public:
		~CylinderCollider();

		void SetColliderScale(float x, float y, float z) override;
		void SetColliderScale(const DirectX::XMFLOAT3& scale) override;

		// Debug Object ����
		void SetDebugMeshData();

	private:
		GInterface::GraphicsDebug* _debugObject;
	};
}

