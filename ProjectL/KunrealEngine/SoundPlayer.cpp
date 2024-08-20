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
	// �ǵ����� ���� �������� ����
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
		assert(0 && "SetVolume() ���带 ���� �� ��� ������ index�� �������� ����");
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
	// ��Ȯ�ϰ� �ؾ� �� �κ� - ���⼭ ã�°� ���� �÷��̾� �� ���� ����Ʈ�� �ε����̴�.
	// ���������� ����ִ� �ε����� ��ȯ�ϴ°� �ƴϴ�.
	// ���ο� �ε����� _soindList[FindIndex("path")].soundIndex �� �����ؾ� �Ѵ�.
	// �� ���� �׷��� �س��� �ϸ� ������ ���̸� ���ؼ�.
	// ���� �ִ� ����Ʈ�� �ش� ���� ������Ʈ�� �������� ���常 �����Ű�
	// ���� �ý��ۿ����� ����Ʈ�� ��� �Ҹ��� �� ��������ٵ�
	// ���� �� ���� �÷��̾� ������Ʈ�� ���� �ý����� �ٸ� ���带 �ǵ帮�� �ʵ��� �ϱ� ���ؼ� ��������.

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
		// �ش� ���� ���� ������ ã���� ���
		return std::distance(vec.begin(), it);
	}
	else
	{
		// �ش� ���� ���� ������ ã������ ���� ���, ���� ó�� ���� ������ �� ����
		return -1; // �Ǵ� �ٸ� ���� ����Ͽ� ���и� ��Ÿ�� �� ����
	}

	return 0;
}
