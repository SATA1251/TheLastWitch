#pragma once
#include "CommonHeader.h"
#include "Component.h"
#include "Collider.h"
#include "GraphicsSystem.h"

/// <summary>
/// 원기둥 모양의 debugMesh는 없으니 관련 기능은 필요 없어보인다
/// 
/// 부모 bone에 붙이는 기능은 없다 필요하면 리건호에게 요청
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

		// Debug Object 설정
		void SetDebugMeshData();

	private:
		GInterface::GraphicsDebug* _debugObject;
	};
}

