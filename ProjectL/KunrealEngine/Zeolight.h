#pragma once
#include "NpcCore.h"
#include "Component.h"

namespace KunrealEngine
{
	class ImageRenderer;

	class Zeolight : public NpcCore , public Component
	{
	public:
	// 생성자
		Zeolight();
		~Zeolight();

		// Component의 가상함수들 오버라이드
		void Initialize() override;
		void Release() override;

		void FixedUpdate() override;
		void Update() override;
		void LateUpdate() override;

		void OnTriggerEnter() override;
		void OnTriggerStay() override;
		void OnTriggerExit() override;

		void SetActive(bool active) override;

		// 세팅 관련
		void SetNpcName(std::string npcName);
		void SetInteractionRange(int Range);
		void SetInterationStep(int step);

		bool GetInteration();

		void stage1();
		void stage2();
		void stage3();
		void stage4();

	public:
		GameObject* image;	// 상호작용 시 출력할 이미지
	};
}

