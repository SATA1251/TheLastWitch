#pragma once
#include "CommonHeader.h"
#include "Component.h"

/// <summary>
///  �÷��̾� R��ų � ���� ǥ�ø� ���� ���� ������Ʈ
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

		// ��밡���� ��ġ��� ���� ���, �Ұ����� ��ġ��� ���� 0.5
		void SetTextureByStatus(bool onMap);

	private:
		Transform* _transform;
		TransparentMesh* _tMesh;
		bool _onCast;					// ��ų ���� ���� ����

		bool CheckisOnMap();

	};

}
