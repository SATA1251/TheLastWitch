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

		friend class Player;		/// 나중에 지워
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
		// wireframe으로 collider의 크기를 표현해줌
		GInterface::GraphicsDebug* _debugObject;

	public:
		// Debug Object 설정
		void SetDebugMeshData();

		// collider 크기를 재조정
		virtual void SetColliderScale(float x, float y, float z) override;
		virtual void SetColliderScale(const DirectX::XMFLOAT3& scale) override;

		/// Mesh Render없이 콜라이더를 특정 본에 붙이기 위한
	public:
		// 부모, 부모의 본을 설정하면 그 본을 부모로 두고 따라 다니게됨
		void SetTransform(GameObject* renderable, std::string boneName);
		// 부모가 있다면 분리하여 부모를 갖지 않도록 함
		void SeperateFromParent();

	private:
		void CalculateParentBone();

	private:
		GameObject* _parentObject;
		std::string _parentBoneName;
	};
}


