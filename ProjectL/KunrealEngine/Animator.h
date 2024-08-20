#pragma once
#include "CommonHeader.h"
#include "Component.h"
#include "GraphicsSystem.h"

namespace KunrealEngine
{
	class _DECLSPEC Animator : public Component
	{
		friend class GameObject;
		// MeshRenderer ������Ʈ�� ���ؼ��� �����ϰ� �ϰ� ������..
	private:
		Animator(GInterface::GraphicsRenderable* meshRenderable);
	public:
		~Animator();

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
		GInterface::GraphicsRenderable* _mesh;

	public:
		// index�� ������ resource ����ҿ� �ִ� clip�� ����
		// speed �⺻�� 10.0, �ݺ����� �⺻���� false
		bool Play(int index, float speed = 10.0f, bool isLoop = false);

		// index�� �ƴ϶� Ŭ�� �̸��� �����ؼ� ���� ����
		bool Play(std::string clipName, float speed = 10.0f, bool isLoop = false);

		// ���� �������� �ִϸ��̼��� ��ž		// �������� 0����
		void Stop();

		// ���� �������� �ִϸ��̼��� �Ͻ�����
		void Pause();

		// �Ͻ����� �� �ִϸ��̼��� �ٽ� ����
		void Replay();

		// ���� �������� �ִϸ��̼��� �� ����������
		float GetCurrentFrame();

		// �ִϸ��̼��� Ư�� ���������� ����
		void SetCurrentFrame(int frame);

		// ���� �������� �ִϸ��̼��� �ƽ� ������ ��������
		float GetMaxFrame();

		// mesh�� � �ִϸ��̼� Ŭ������ ����ִ��� Ȯ��
		const std::vector<std::string>& GetClipNames();

		// ���� �������� �ִϸ��̼� �̸� ��������
		const std::string& GetNowAnimationName();

		// �ִϸ��̼� ���������� ���� ��������
		bool GetIsAnimationPlaying();
	};
}

