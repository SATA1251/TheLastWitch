#pragma once

#include <windows.h>
#include <string>
namespace ArkEngine
{
	class __declspec(dllexport) Utils
	{
	public:
		Utils() {}
		~Utils() {}

	public:
		static bool StartsWith(std::string str, std::string comp);
		static bool StartsWith(std::wstring str, std::wstring comp);

		static void Replace(OUT std::string& str, std::string comp, std::string rep);
		static void Replace(OUT std::wstring& str, std::wstring comp, std::wstring rep);

		std::wstring ToWString(std::string value);
		std::string ToString(std::wstring value);

		// wstring을 const char*로 변환하는 함수
		//const char* WstringToChar(const std::wstring& wstr);
	};

}