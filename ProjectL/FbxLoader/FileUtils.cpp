#include "FileUtils.h"


ArkEngine::FileUtils::FileUtils()
	: _handle(nullptr)
{
}

ArkEngine::FileUtils::~FileUtils()
{
	if (_handle != INVALID_HANDLE_VALUE)
	{
		::CloseHandle(_handle);
		_handle = INVALID_HANDLE_VALUE;
	}
}

void ArkEngine::FileUtils::Open(std::wstring filePath, FileMode mode)
{
	if (mode == FileMode::Write)
	{
		_handle = ::CreateFile(
			filePath.c_str(),
			GENERIC_WRITE,
			0,
			nullptr,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			nullptr
		);
	}
	else
	{
		_handle = ::CreateFile(
			filePath.c_str(),
			GENERIC_READ,
			FILE_SHARE_READ,
			nullptr,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			nullptr
		);
	}


	assert(_handle != INVALID_HANDLE_VALUE);
}

void ArkEngine::FileUtils::Write(void* data, unsigned int dataSize)
{
	unsigned int numOfBytes = 0;
	WriteFile(_handle, data, dataSize, reinterpret_cast<LPDWORD>(&numOfBytes), nullptr);
	//assert(::WriteFile(_handle, data, dataSize, reinterpret_cast<LPDWORD>(&numOfBytes), nullptr));
}


void ArkEngine::FileUtils::Write(const std::string& data)
{
	unsigned int size = (unsigned int)data.size();
	Write(size);

	if (data.size() == 0)
		return;

	Write((void*)data.data(), size);
}

void ArkEngine::FileUtils::Read(void** data, unsigned int dataSize)
{
	unsigned int numOfBytes = 0;
	ReadFile(_handle, *data, dataSize, reinterpret_cast<LPDWORD>(&numOfBytes), nullptr);
	//assert(::ReadFile(_handle, *data, dataSize, reinterpret_cast<LPDWORD>(&numOfBytes), nullptr));
}

void ArkEngine::FileUtils::Read(OUT std::string& data)
{
	unsigned int size = Read<unsigned int>();

	if (size == 0)
		return;

	// size 만큼 동적 할당
	char* temp = new char[size + 1];
	temp[size] = 0;
	Read((void**)&temp, size);
	data = temp;
	delete[] temp;
}
