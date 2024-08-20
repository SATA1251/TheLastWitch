#pragma once
#include "CommonHeader.h"
#include "Component.h"

/// <summary>
/// ���� ������ ������ְ� ��� ���θ� �������ִ� ������Ʈ
/// 
/// ���� ���带 ����� �� �ִµ�, ������ ����ڴ� � ������ ������Ʈ�� ���° �������� �˰� �־����
/// �����͸� ���� Ȯ���ϸ� ū ���� ������ 
///
/// 2024.03.13
/// - RemoveSound �Լ� �� change3Dmode �Լ� �߰�
/// </summary>

namespace KunrealEngine
{
	class _DECLSPEC SoundPlayer : public Component
	{
		friend class GameObject;
	private:
		SoundPlayer();
	public:
		~SoundPlayer();

		virtual void Initialize() override;
		virtual void Release() override;

		virtual void FixedUpdate() override;
		virtual void Update() override;
		virtual void LateUpdate() override;

		virtual void OnTriggerEnter() override;
		virtual void OnTriggerStay() override;
		virtual void OnTriggerExit() override;

		virtual void SetActive(bool active) override;

	public:
		// �Ҹ� ��� ������ ���� ����ü
		struct SoundInfo
		{
			std::string _soundPath;			// ���� ���� ���
			bool _is3DSound;				// 3D ��������					// �⺻�� false
			bool _isLoop;					// �ݺ��Ǵ� ��������			// �⺻�� false
			int _volume;
			int _soundIndex;				// SoundSystem ���ο��� �ٷ�� index	// �� ������Ʈ�� index�ʹ� �ٸ��� ����
			unsigned int _innerIndex;

			bool operator==(const SoundInfo& other) const {
				// ��� ��� ������ ������ ���մϴ�.
				return _soundPath == other._soundPath &&
					_is3DSound == other._is3DSound &&
					_isLoop == other._isLoop &&
					_volume == other._volume &&
					_soundIndex == other._soundIndex &&
					_innerIndex == other._innerIndex;
			}
		};

	public:
		int _createdCnt;					// ���带 � ���������
		std::vector<SoundInfo> _soundList;	// ���� ������ ���� �����̳�

	public:
		void RemoveSound(int index);

		// soundInfo�� ���� �� _is3DSound ���� �ٲ�� ����Ǵ� �Լ�.
		void Change3Dmode(int index);

		// soundInfo�� ���� �� _isLoop ���� ������ �� ��� �ϴ� �Լ�
		void ChangeisLoop(int index);

		//	soundInfo�� ���� �� _soundPath ���� �ٲ�� ����Ǵ� �Լ�.
		void ChangeFilePath(int index, std::string soundFile);

		// ���������� �����. ������ ���带 ����°� �ƴϰ�.
		int CreateSoundInfo(std::string soundFile, bool is3D = false, bool isLoop = true, int volume = 100);
		void CreateSound(int soundindex, int sound_type);

		// ���带 ������ ��� ���� ���� index�� ��������Ѵ�
		// �ϳ��� ���� ��� ������ �Ű�������
		void SetVolume(int vol, int index = -1);

		// �Է��� �ε����� �ش��ϴ� ���带 ���		// ù��° ����� 0�� �ε���
		// ���� �ý��� ���� ����Ʈ�� �ִ°� �ƴ϶� ���� �÷��̾� ������Ʈ ���� ����Ʈ �ε����� �ִ´�
		void Play(int index);

		// �Է��� �ε����� �ش��ϴ� ���带 �Ͻ�����
		void Pause(int index);

		// �Է��� �ε����� �ش��ϴ� ���带 ����
		void Stop(int index);

		// ��� ���� ��� ����
		void StopAll();

		// �ε��� Ž��
		int FindIndex(std::string soundname);
		
		// SoundInfo�� �� �迭�� �־ �ش� SoundInfo�� �Է¹��� �迭�� �ִ����� Ž���ϴ� �Լ�
		// �̰� ���ο��� ������ ����� ���� ���� �� �ּ��� �޾Ƴ����� �ƴ϶� � �뵵�� ��������� �𸣰ڴ�.
		int FindIndex(const std::vector<SoundInfo>& vec, SoundInfo value);
	};
}