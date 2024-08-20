#pragma once
#include <string>
#include <windows.h>
#include <cassert>

enum FileMode : unsigned int
{
	Write,
	Read,
};

namespace ArkEngine
{
	class __declspec(dllexport) FileUtils
	{
	public:
		FileUtils();
		~FileUtils();

		void Open(std::wstring filePath, FileMode mode);

		template<typename T>
		void Write(const T& data)
		{
			DWORD numOfBytes = 0;
			WriteFile(_handle, &data, sizeof(T), (LPDWORD)&numOfBytes, nullptr);
			//assert(::WriteFile(_handle, &data, sizeof(T), (LPDWORD)&numOfBytes, nullptr));
		}

		template<>
		void Write<std::string>(const std::string& data)
		{
			return Write(data);
		}

		void Write(void* data, unsigned int dataSize);
		void Write(const std::string& data);

		template<typename T>
		void Read(OUT T& data)
		{
			DWORD numOfBytes = 0;
			ReadFile(_handle, &data, sizeof(T), (LPDWORD)&numOfBytes, nullptr);
			//assert(::ReadFile(_handle, &data, sizeof(T), (LPDWORD)&numOfBytes, nullptr));
		}

		template<typename T>
		T Read()
		{
			T data;
			Read(data);
			return data;
		}

		void Read(void** data, unsigned int dataSize);
		void Read(OUT std::string& data);

	private:

		HANDLE _handle = INVALID_HANDLE_VALUE;
	};
}
