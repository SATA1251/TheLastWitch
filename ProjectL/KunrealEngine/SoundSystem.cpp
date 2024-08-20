#include "SoundSystem.h"
#include <assert.h>
#include <algorithm>
#include <typeinfo>
#include <filesystem>

#pragma comment (lib, "fmod_vc.lib")
#pragma comment (lib, "fmodL_vc.lib")

#define ASsert(formula, message) assert(formula && message)
#define Assert(message) assert(0 && message)

namespace KunrealEngine
{
	SoundSystem::SoundSystem() : _fmodSystem(nullptr), DISTANCEFACTOR(1.0f)
	{
		_channels.resize(512, nullptr);
	}

	SoundSystem::SoundSystem(const SoundSystem& soundClass)
		: _fmodSystem(nullptr), DISTANCEFACTOR(1.0f)
	{
		_channels.resize(512, nullptr);
	}

	SoundSystem::~SoundSystem()
	{
		Release();

	}

	/////////////////////////////////
	/////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////

	SoundSystem& SoundSystem::GetInstance()
	{
		static SoundSystem* instance;

		if (instance == nullptr)
		{
			instance = new SoundSystem();
		}
		return *instance;
	}

	bool SoundSystem::Initialize(HWND hWnd)
	{
		FMOD_RESULT result;
		result = FMOD::System_Create(&_fmodSystem);
		if (result != FMOD_OK)
		{
			std::cerr << "FMOD error! (" << result << ") " << FMOD_ErrorString(result) << std::endl;
			return false;
		}

		result = _fmodSystem->init(512, FMOD_INIT_NORMAL, nullptr);
		if (result != FMOD_OK)
		{
			std::cerr << "FMOD error! (" << result << ") " << FMOD_ErrorString(result) << std::endl;
			return false;
		}
		
		result = _fmodSystem->set3DSettings(1.0f, DISTANCEFACTOR, 1.0f);
		if (result != FMOD_OK)
		{
			std::cerr << "FMOD error! (" << result << ") " << FMOD_ErrorString(result) << std::endl;
			return false;
		}

		return true;
	}

	void SoundSystem::Release()
	{
		for (auto& sound : _soundBuffer)
		{
			if (sound.fmodSound)
			{
				sound.fmodSound->release();
			}
		}
		_fmodSystem->close();
		_fmodSystem->release();
	}

	/////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////

	int SoundSystem::AddSound(std::string filename, int volume, SOUNDTYPE type)
	{
		Sound newSound;
		newSound.fileName = filename;
		newSound.volume = volume;
		newSound.type = type;
		FMOD::Sound* fmodSound;
		FMOD_RESULT result = _fmodSystem->createSound(filename.c_str(), FMOD_DEFAULT, 0, &fmodSound);
		if (result != FMOD_OK)
		{
			std::cerr << "FMOD error! (" << result << ") " << FMOD_ErrorString(result) << std::endl;
			return -1;
		}
		newSound.fmodSound = fmodSound;
		_soundBuffer.push_back(newSound);
		return _soundBuffer.size() - 1;
	}

	int SoundSystem::Add3DSound(std::string filename, int volume, SOUNDTYPE type, int xpos /*= 0*/, int ypos /*= 0*/, int zpos /*= 0*/)
	{
		Sound newSound;
		newSound.fileName = filename;
		newSound.volume = volume;
		newSound.type = type;
		FMOD::Sound* fmodSound;
		FMOD_RESULT result = _fmodSystem->createSound(filename.c_str(), FMOD_3D, 0, &fmodSound);
		if (result != FMOD_OK)
		{
			std::cerr << "FMOD error! (" << result << ") " << FMOD_ErrorString(result) << std::endl;
			return -1;
		}
		result = fmodSound->set3DMinMaxDistance(1.0f * DISTANCEFACTOR, 5000.0f * DISTANCEFACTOR);
		if (result != FMOD_OK)
		{
			std::cerr << "FMOD error! (" << result << ") " << FMOD_ErrorString(result) << std::endl;
			return -1;
		}
		newSound.fmodSound = fmodSound;
		_soundBuffer.push_back(newSound);
		return _soundBuffer.size() - 1;
	}

	void SoundSystem::Update()
	{
		_fmodSystem->update();
	}

	void SoundSystem::RemoveSound(int index)
	{
		if (index >= 0 && index < _soundBuffer.size())
		{
			if (_soundBuffer[index].fmodSound) {
				_soundBuffer[index].fmodSound->release();
			}
			_soundBuffer.erase(_soundBuffer.begin() + index);
		}
	}

	void SoundSystem::ClearAllSound()
	{
		for (auto& sound : _soundBuffer)
		{
			if (sound.fmodSound)
			{
				sound.fmodSound->release();
			}
		}
		_soundBuffer.clear();
	}

	void SoundSystem::SetVolume(int index, int volume)
	{
		if (index >= 0 && index < _soundBuffer.size())
		{
			_soundBuffer[index].volume = volume;
			if (_channels[index])
			{
				_channels[index]->setVolume(volume / 100.0f);
			}
		}
	}

	void SoundSystem::SetvolumeGroup(SOUNDTYPE type, int volume)
	{
		volume *= 10;

		for (auto iter = 0; iter < _soundBuffer.size() ; iter++)
		{
			if (_soundBuffer[iter].type == type)
			{
				if (_channels[iter])
				{
					_channels[iter]->setVolume(volume / 100.0f);
				}
				_soundBuffer[iter].volume = volume;
			}
		}
	}

	int SoundSystem::Change3DorMono(int index)
	{
		if (index >= 0 && index < _soundBuffer.size())
		{
			FMOD_MODE currentMode;
			_soundBuffer[index].fmodSound->getMode(&currentMode);
			if (currentMode & FMOD_3D)
			{
				_soundBuffer[index].fmodSound->setMode(FMOD_DEFAULT);
				return 0; // Changed to mono
			}
			else
			{
				_soundBuffer[index].fmodSound->setMode(FMOD_3D);
				return 1; // Changed to 3D
			}
		}
		return -1;
	}

	int SoundSystem::GetSoundListSize()
	{
		return _soundBuffer.size();
	}

	std::vector<std::string> KunrealEngine::SoundSystem::GetSoundPathList()
	{
		namespace fs = std::filesystem;
		std::string directory = "Resources/Sound";
		std::vector<std::string> fileNames;

		// Iterate over the files in the directory
		for (const auto& entry : fs::directory_iterator(directory)) {
			// Check if it's a regular file
			if (fs::is_regular_file(entry)) {
				// Get the filename from the path and add it to the vector
				fileNames.push_back(entry.path().filename().string());
			}
		}

		return fileNames;
	}

	/////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////

	void SoundSystem::updateSoundPosition(int index, float x, float y, float z)
	{
		if (index >= 0 && index < _soundBuffer.size())
		{
			FMOD_VECTOR pos = { x, y, z };
			FMOD_VECTOR vel = { 0.0f, 0.0f, 0.0f };
			auto result = _channels[index]->set3DAttributes(&pos, &vel);
			if (result != FMOD_OK)
			{
				std::cerr << "FMOD error! (" << result << ") " << FMOD_ErrorString(result) << std::endl;
				return;
			}
		}
	}

	void SoundSystem::updateListenerPosition(float x, float y, float z)
	{
  		FMOD_VECTOR pos =		{ x, y, z };
		FMOD_VECTOR vel =		{ 0.0f, 0.0f, 0.0f };
		FMOD_VECTOR forward =	{ 0.0f, 0.0f, -1.0f };
		FMOD_VECTOR up =		{ 0.0f, 1.0f, 0.0f };
		auto result = _fmodSystem->set3DListenerAttributes(0, &pos, &vel, &forward, &up);
		if (result != FMOD_OK)
		{
			std::cerr << "FMOD error! (" << result << ") " << FMOD_ErrorString(result) << std::endl;
			return;
		}
	}

	void SoundSystem::Play(int index)
	{
		FMOD_RESULT fr;

		if (index >= 0 && index < _soundBuffer.size())
		{
 			bool isplay = false;
 			auto result = _channels[index]->isPlaying(&isplay);
 			if (isplay) 
 			{
 				return;
 			}

			fr = _fmodSystem->playSound(_soundBuffer[index].fmodSound, 0, false, &_channels[index]);
			played++;
			_channels2.push_back(_channels[index]);
			if (fr != FMOD_OK)
			{
				std::cerr << "FMOD error! (" << fr << ") " << FMOD_ErrorString(fr) << std::endl;
				return;
			}
			if (_channels[index])
			{
				FMOD_MODE currentMode;
				_soundBuffer[index].fmodSound->getMode(&currentMode);
				if (currentMode & FMOD_3D)
				{
					FMOD_VECTOR pos = { 0.0f, 0.0f, 0.0f };
					FMOD_VECTOR vel = { 0.0f, 0.0f, 0.0f };
					_channels[index]->set3DAttributes(&pos, &vel);
				}
				_channels[index]->setVolume(_soundBuffer[index].volume / 100.0f);
			}
		}
	}

	void SoundSystem::PlayWithPos(int index, int xpos, int ypos, int zpos)
	{
		if (index >= 0 && index < _soundBuffer.size())
		{
			updateSoundPosition(index, xpos, ypos, zpos);
			Play(index);
		}
	}

	void SoundSystem::Loop(int index)
	{
		FMOD_RESULT fr;

		if (index >= 0 && index < _soundBuffer.size())
		{
			fr = _soundBuffer[index].fmodSound->setMode(FMOD_LOOP_NORMAL);
			Play(index);
		}
	}

	void SoundSystem::Pause(int index)
	{
		if (index >= 0 && index < _soundBuffer.size() && _channels[index])
		{
			bool isPaused;
			_channels[index]->getPaused(&isPaused);
			_channels[index]->setPaused(!isPaused);
		}
	}

	void SoundSystem::Stop(int index)
	{
		if (index >= 0 && index < _soundBuffer.size() && _channels[index])
		{
			_channels[index]->stop();
		}
	}
}