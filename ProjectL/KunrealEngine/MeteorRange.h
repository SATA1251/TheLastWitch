#pragma once
#include "CommonHeader.h"
#include "Component.h"

/// <summary>
///  플레이어 R스킬 운석 범위 표시를 위한 보조 컴포넌트
/// </summary>

namespace KunrealEngine
{
	class Transform;
	class TransparentMesh;
	class Particle;

	class _DECLSPEC MeteorRange : public Component
	{
		friend class GameObject;
		friend class PlayerAbility;
	private:
		MeteorRange();

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
		~MeteorRange();

		// 사용가능한 위치라면 원본 출력, 불가능한 위치라면 투명도 0.5
		void SetTextureByStatus(bool onMap);

	private:
		Transform* _transform;
		TransparentMesh* _tMesh;
		bool _onCast;					// 스킬 시전 성공 여부

		bool CheckisOnMap();

	};

}
