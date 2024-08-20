#pragma once

#include "CommonHeader.h"
#include "Component.h"
#include "Collider.h"
#include "GraphicsSystem.h"

/// <summary>
/// FBX 모양의 collider를 생성해주는 컴포넌트
/// 
/// Mesh 모양의 collider shape를 physx에서 미리 만들어놔야 하기 때문에
/// 새로운 모양이 필요하면 요청바람
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

		// Debug Object 설정
		void SetDebugMeshData();
	};
}

