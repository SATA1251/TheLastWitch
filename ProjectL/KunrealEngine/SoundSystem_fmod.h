#pragma once
#include <windows.h>
#include <iostream>
#include <mmsystem.h>
#include <stdio.h>
#include <assert.h>

#include <vector>
#include <string>

#include "CommonHeader.h"
#include "../Inc/fmod/fmod.hpp"
#include "../Inc/fmod/fmod_errors.h"

using namespace FMOD;

namespace KunrealEngine
{
	enum class SOUNDTYPE
	{
		BGM = 0,
		SFX = 1
	};

	// 사운드 정보 구조체
	struct _DECLSPEC Sound
	{
		virtual ~Sound() {}
		std::string fileName;
		int volume = 0; // 0~100
		FMOD::Sound* fmodSound;
		SOUNDTYPE type = SOUNDTYPE::SFX;
	};

	// SoundSystem 클래스
	class _DECLSPEC SoundSystem
	{
	public:
		static SoundSystem& GetInstance();

		bool Initialize(HWND hWnd);

		void Release();

		int AddSound(std::string& filename, int volume);

		int Add3DSound(std::string& filename, int volume, int xpos = 0, int ypos = 0, int zpos = 0);

		void RemoveSound(int index);

		void ClearAllSound();

		void SetVolume(int index, int volume);

		void SetvolumeGroup(SOUNDTYPE type, int volume);

		int Change3DorMono(int index);

		int GetSoundListSize();

		std::vector<std::string> GetSoundPathList();

		void updateSoundPosition(int index, float x, float y, float z);

		void updateListenerPosition(float x, float y, float z);

		void Play(int index);

		void PlayWithPos(int index, int xpos, int ypos, int zpos);

		void Loop(int index);

		void Pause(int index);

		void Stop(int index);

	private:
		SoundSystem();
		SoundSystem(const SoundSystem& soundClass);
		~SoundSystem();

		FMOD::System* _fmodSystem;
		std::vector<Sound> _soundBuffer;
		std::vector<FMOD::Channel*> _channels;
	};

	// Usage example:
	// int main() {
	//     SoundSystem& soundSystem = SoundSystem::GetInstance();
	//     soundSystem.Initialize(GetConsoleWindow());
	//     int soundIndex = soundSystem.AddSound("example.wav", 50);
	//     soundSystem.Play(soundIndex);
	//     soundSystem.Release();
	//     return 0;
	// }
}