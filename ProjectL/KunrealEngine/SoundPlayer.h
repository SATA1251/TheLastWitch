#pragma once
#include "CommonHeader.h"
#include "Component.h"

/// <summary>
/// 사운드 파일을 등록해주고 출력 여부를 결정해주는 컴포넌트
/// 
/// 여러 사운드를 등록할 수 있는데, 컨텐츠 담당자는 어떤 파일이 컴포넌트의 몇번째 사운드인지 알고 있어야함
/// 에디터를 통해 확인하면 큰 문제 없을듯 
///
/// 2024.03.13
/// - RemoveSound 함수 및 change3Dmode 함수 추가
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
		// 소리 출력 조건을 위한 구조체
		struct SoundInfo
		{
			std::string _soundPath;			// 사운드 파일 경로
			bool _is3DSound;				// 3D 사운드인지					// 기본은 false
			bool _isLoop;					// 반복되는 사운드인지			// 기본은 false
			int _volume;
			int _soundIndex;				// SoundSystem 내부에서 다루는 index	// 이 컴포넌트의 index와는 다르니 주의
			unsigned int _innerIndex;

			bool operator==(const SoundInfo& other) const {
				// 모든 멤버 변수가 같은지 비교합니다.
				return _soundPath == other._soundPath &&
					_is3DSound == other._is3DSound &&
					_isLoop == other._isLoop &&
					_volume == other._volume &&
					_soundIndex == other._soundIndex &&
					_innerIndex == other._innerIndex;
			}
		};

	public:
		int _createdCnt;					// 사운드를 몇개 만들었는지
		std::vector<SoundInfo> _soundList;	// 사운드 정보에 관한 컨테이너

	public:
		void RemoveSound(int index);

		// soundInfo내 변수 중 _is3DSound 값이 바뀌면 실행되는 함수.
		void Change3Dmode(int index);

		// soundInfo내 변수 중 _isLoop 값을 변경할 때 사용 하는 함수
		void ChangeisLoop(int index);

		//	soundInfo내 변수 중 _soundPath 값이 바뀌면 실행되는 함수.
		void ChangeFilePath(int index, std::string soundFile);

		// 사운드인포를 만든다. 실제로 사운드를 만드는게 아니고.
		int CreateSoundInfo(std::string soundFile, bool is3D = false, bool isLoop = true, int volume = 100);
		void CreateSound(int soundindex, int sound_type);

		// 사운드를 여러개 들고 있을 때는 index를 정해줘야한다
		// 하나만 있을 경우 볼륨만 매개변수로
		void SetVolume(int vol, int index = -1);

		// 입력한 인덱스에 해당하는 사운드를 출력		// 첫번째 사운드는 0번 인덱스
		// 사운드 시스템 내의 리스트를 넣는게 아니라 사운드 플레이어 컴포넌트 내의 리스트 인덱스를 넣는다
		void Play(int index);

		// 입력한 인덱스에 해당하는 사운드를 일시정지
		void Pause(int index);

		// 입력한 인덱스에 해당하는 사운드를 종료
		void Stop(int index);

		// 모든 사운드 출력 종료
		void StopAll();

		// 인덱스 탐색
		int FindIndex(std::string soundname);
		
		// SoundInfo와 그 배열을 넣어서 해당 SoundInfo가 입력받은 배열에 있는지를 탐색하는 함수
		// 이건 내부에서 쓰려고 만든거 같음 만들 때 주석을 달아놓은게 아니라 어떤 용도로 만들었는지 모르겠다.
		int FindIndex(const std::vector<SoundInfo>& vec, SoundInfo value);
	};
}