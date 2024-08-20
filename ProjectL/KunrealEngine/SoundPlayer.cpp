#include "SoundPlayer.h"
#include "SoundSystem.h"

KunrealEngine::SoundPlayer::SoundPlayer()
	:_createdCnt(0)
{

}

KunrealEngine::SoundPlayer::~SoundPlayer()
{

}

void KunrealEngine::SoundPlayer::Initialize()
{

}

void KunrealEngine::SoundPlayer::Release()
{

}

void KunrealEngine::SoundPlayer::FixedUpdate()
{

}

void KunrealEngine::SoundPlayer::Update()
{

}

void KunrealEngine::SoundPlayer::LateUpdate()
{

}

void KunrealEngine::SoundPlayer::OnTriggerEnter()
{

}

void KunrealEngine::SoundPlayer::OnTriggerStay()
{

}

void KunrealEngine::SoundPlayer::OnTriggerExit()
{

}

void KunrealEngine::SoundPlayer::SetActive(bool active)
{
	this->_isActivated = active;
}

void KunrealEngine::SoundPlayer::RemoveSound(int index)
{
	if (_soundList.size() == 0)
	{
		return;
	}

	int removedindex = _soundList[index]._soundIndex;

	SoundSystem::GetInstance().RemoveSound(_soundList[index]._soundIndex);

	for (auto soundindex = _soundList.begin() + removedindex; soundindex > _soundList.end(); soundindex + 1)
	{
		soundindex->_soundIndex -= 1;
	}

	_soundList.erase(_soundList.begin() + index);
}

void KunrealEngine::SoundPlayer::Change3Dmode(int index)
{
	Stop(_soundList[index]._soundIndex);

	SoundSystem::GetInstance().Change3DorMono(_soundList[index]._soundIndex);

	if (_soundList[index]._is3DSound)
	{
		_soundList[index]._is3DSound = false;
	}

	else if (!_soundList[index]._is3DSound)
	{
		_soundList[index]._is3DSound = true;
	}
}

void KunrealEngine::SoundPlayer::ChangeisLoop(int index)
{
	Stop(_soundList[index]._soundIndex);

	_soundList[index]._isLoop = _soundList[index]._isLoop ? false : true;
}

void KunrealEngine::SoundPlayer::ChangeFilePath(int index, std::string soundFile)
{
	// 의도하지 않은 동작으로 보류
// 	SoundSystem::GetInstance().Stop(_soundList[index]._soundIndex);
// 
// 	if (_soundList.size() == 0)
// 	{
// 		return;
// 	}
// 
// 	int removedindex = _soundList[index]._soundIndex;
// 
// 	SoundSystem::GetInstance().RemoveSound(_soundList[index]._soundIndex);
// 
// 	for (auto soundindex = _soundList.begin() + removedindex; soundindex > _soundList.end(); soundindex + 1)
// 	{
// 		soundindex->_soundIndex -= 1;
// 	}
// 
// 	_soundList[index]._soundPath = soundFile;
// 	CreateSound(index);
// 	_soundList[index]._innerIndex = _soundList.size();
}

int KunrealEngine::SoundPlayer::CreateSoundInfo(std::string soundFile, bool is3D /*= false*/, bool isLoop /*= true*/, int volume /*= 100*/)
{
	//SoundInfo newSound(soundFile, is3D, isLoop, volume, _soundList.size());
	SoundInfo newSound = { soundFile, is3D, isLoop, volume, -1, (unsigned int)_soundList.size() };

	_soundList.push_back(newSound);
	int returnValue = FindIndex(_soundList, newSound);
	return returnValue;
}

void KunrealEngine::SoundPlayer::CreateSound(int soundindex, int sound_type = 1)
{
	if (_soundList[soundindex]._is3DSound)
	{
		_soundList[soundindex]._soundIndex = SoundSystem::GetInstance().Add3DSound(_soundList[soundindex]._soundPath, _soundList[soundindex]._volume, (SOUNDTYPE)sound_type);
	}
	else
	{
		_soundList[soundindex]._soundIndex = SoundSystem::GetInstance().AddSound(_soundList[soundindex]._soundPath, _soundList[soundindex]._volume, (SOUNDTYPE)sound_type);
	}
}

void KunrealEngine::SoundPlayer::SetVolume(int vol, int index /*= 0*/)
{
	if (_soundList.size() > 1 && index == -1)
	{
		assert(0 && "SetVolume() 사운드를 여러 개 들고 있지만 index를 정해주지 않음");
	}
	else if (index == -1)
	{
		index = 0;
	}

	SoundSystem::GetInstance().SetVolume(_soundList[index]._soundIndex, vol);
}

void KunrealEngine::SoundPlayer::Play(int index)
{
	bool isLoop = _soundList[index]._isLoop;

	if (!isLoop)
	{
		SoundSystem::GetInstance().Play(_soundList[index]._soundIndex);
	}
	else
	{
		SoundSystem::GetInstance().Loop(_soundList[index]._soundIndex);
	}
}

void KunrealEngine::SoundPlayer::Pause(int index)
{
	SoundSystem::GetInstance().Pause(_soundList[index]._soundIndex);
}

void KunrealEngine::SoundPlayer::Stop(int index)
{
	SoundSystem::GetInstance().Stop(_soundList[index]._soundIndex);
}

void KunrealEngine::SoundPlayer::StopAll()
{
	for (auto iter = _soundList.begin(); iter != _soundList.end(); iter++)
	{
		SoundSystem::GetInstance().Stop(iter->_soundIndex);
	}
}

int KunrealEngine::SoundPlayer::FindIndex(std::string soundname)
{
	// 명확하게 해야 할 부분 - 여기서 찾는건 사운드 플레이어 내 사운드 리스트의 인덱스이다.
	// 사운드인포에 들어있는 인덱스를 반환하는게 아니다.
	// 내부에 인덱스는 _soindList[FindIndex("path")].soundIndex 로 접근해야 한다.
	// 왜 굳이 그렇게 해놨냐 하면 관리의 용이를 위해서.
	// 여기 있는 리스트는 해당 게임 오브젝트가 관리해줄 사운드만 있을거고
	// 사운드 시스템에서의 리스트는 모든 소리가 다 들어있을텐데
	// 굳이 이 사운드 플레이어 컴포넌트가 사운드 시스템의 다른 사운드를 건드리지 않도록 하기 위해서 나눠놨다.

	int result = 0;

	for (SoundInfo sound : _soundList)
	{
		if (sound._soundPath == soundname)
		{
			return result;
		}
		result++;
	}

	return -1;
}


int KunrealEngine::SoundPlayer::FindIndex(const std::vector<SoundInfo>& vec, SoundInfo value)
{
	auto it = std::find(vec.begin(), vec.end(), value);

	if (it != vec.end())
	{
		// 해당 값이 벡터 내에서 찾아진 경우
		return std::distance(vec.begin(), it);
	}
	else
	{
		// 해당 값이 벡터 내에서 찾아지지 않은 경우, 예외 처리 등을 수행할 수 있음
		return -1; // 또는 다른 값을 사용하여 실패를 나타낼 수 있음
	}

	return 0;
}
