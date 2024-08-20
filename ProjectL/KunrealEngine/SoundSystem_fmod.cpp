#include "SoundSystem_fmod.h"
#include <assert.h>
#include <algorithm>
#include <typeinfo>
#include <filesystem>

#define ASsert(formula, message) assert(formula && message)
#define Assert(message) assert(0 && message)

namespace KunrealEngine
{ 
	SoundSystem::SoundSystem() : _fmodSystem(nullptr)
	{
		_channels.resize(512, nullptr);
	}

	SoundSystem::SoundSystem(const SoundSystem& soundClass)
		: _fmodSystem(nullptr)
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

	int SoundSystem::AddSound(std::string& filename, int volume)
	{
		Sound newSound;
		newSound.fileName = filename;
		newSound.volume = volume;
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

	int SoundSystem::Add3DSound(std::string& filename, int volume, int xpos, int ypos, int zpos)
	{
		Sound newSound;
		newSound.fileName = filename;
		newSound.volume = volume;
		FMOD::Sound* fmodSound;
		FMOD_RESULT result = _fmodSystem->createSound(filename.c_str(), FMOD_3D, 0, &fmodSound);
		if (result != FMOD_OK)
		{
			std::cerr << "FMOD error! (" << result << ") " << FMOD_ErrorString(result) << std::endl;
			return -1;
		}
		newSound.fmodSound = fmodSound;
		_soundBuffer.push_back(newSound);
		updateSoundPosition(_soundBuffer.size() - 1, xpos, ypos, zpos);
		return _soundBuffer.size() - 1;
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
			if (_channels[index]) {
				_channels[index]->setVolume(volume / 100.0f);
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
			if (_channels[index])
			{
				_channels[index]->set3DAttributes(&pos, &vel);
			}
		}
	}

	void SoundSystem::updateListenerPosition(float x, float y, float z)
	{
		FMOD_VECTOR pos = { x, y, z };
		FMOD_VECTOR vel = { 0.0f, 0.0f, 0.0f };
		FMOD_VECTOR forward = { 0.0f, 0.0f, 1.0f };
		FMOD_VECTOR up = { 0.0f, 1.0f, 0.0f };
		_fmodSystem->set3DListenerAttributes(0, &pos, &vel, &forward, &up);
	}

	void SoundSystem::Play(int index)
	{
		FMOD_RESULT fr;

		if (index >= 0 && index < _soundBuffer.size())
		{
			FMOD::Channel* channel = nullptr;
			fr = _fmodSystem->playSound(_soundBuffer[index].fmodSound, 0, false, &channel);
			if (channel)
			{
				channel->setVolume(_soundBuffer[index].volume / 100.0f);
				_channels[index] = channel;
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