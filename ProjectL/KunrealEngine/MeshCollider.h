#pragma once

#include "CommonHeader.h"
#include "Component.h"
#include "Collider.h"
#include "GraphicsSystem.h"

/// <summary>
/// FBX ����� collider�� �������ִ� ������Ʈ
/// 
/// Mesh ����� collider shape�� physx���� �̸� �������� �ϱ� ������
/// ���ο� ����� �ʿ��ϸ� ��û�ٶ�
/// </summary>

namespace KunrealEngine
{
	class _DECLSPEC MeshCollider : public Component, public Collider
	{
		friend class GameObject;
	private:
		MeshCollider();

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
		GInterface::GraphicsDebug* _debugObject;
		std::string _meshName;

	public:
		~MeshCollider();

		void CreateMeshCollider(std::string meshName);

		void SetColliderScale(float x, float y, float z) override;
		void SetColliderScale(const DirectX::XMFLOAT3& scale) override;

		// Debug Object ����
		void SetDebugMeshData();
	};
}

