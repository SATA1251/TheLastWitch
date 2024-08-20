#pragma once
#include "NpcCore.h"
#include "Component.h"

namespace KunrealEngine
{
	class ImageRenderer;

	class Zeolight : public NpcCore , public Component
	{
	public:
	// ������
		Zeolight();
		~Zeolight();

		// Component�� �����Լ��� �������̵�
		void Initialize() override;
		void Release() override;

		void FixedUpdate() override;
		void Update() override;
		void LateUpdate() override;

		void OnTriggerEnter() override;
		void OnTriggerStay() override;
		void OnTriggerExit() override;

		void SetActive(bool active) override;

		// ���� ����
		void SetNpcName(std::string npcName);
		void SetInteractionRange(int Range);
		void SetInterationStep(int step);

		bool GetInteration();

		void stage1();
		void stage2();
		void stage3();
		void stage4();

	public:
		GameObject* image;	// ��ȣ�ۿ� �� ����� �̹���
	};
}

