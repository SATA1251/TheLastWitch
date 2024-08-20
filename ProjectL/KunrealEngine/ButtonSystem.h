#pragma once
#include "Component.h"
#include "functional"

namespace KunrealEngine
{
	class ImageRenderer;
	class SoundPlayer;

	class ButtonSystem : public Component
	{
	public:
		ButtonSystem();
		~ButtonSystem();

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

		// ��ư ���۽� ������ �Լ��� ����
		void SetButtonFunc(std::function<void()> func);
		void SetImage(ImageRenderer* image);
		void Setfocused(ImageRenderer* focusimage);

	private: 
		ImageRenderer* _image;
		ImageRenderer* _focusimage;

		// ���� ��� ������Ʈ
		SoundPlayer* _soundComp;
		// �Է��ص� �Ҹ� (int�� ���� �ε����θ� �����Ѵ�.)
		int _soundclick;

		// �̹����� ��Ŀ�������� �ƴ��� �����ϱ� ���� ����
		// �������� ���̹����� �ϸ� �ӵ��� �峭 �ƴҰ� ���Ƽ� �߰�
		bool _focused;

		std::function<void()> _script;
	};
}