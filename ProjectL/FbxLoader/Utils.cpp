#include "Utils.h"

#include <iostream>
#include <locale>
#include <codecvt>

bool ArkEngine::Utils::StartsWith(std::string str, std::string comp)
{
	std::string::size_type index = str.find(comp);
	if (index != std::string::npos && index == 0)
		return true;

	return false;
}

bool ArkEngine::Utils::StartsWith(std::wstring str, std::wstring comp)
{
	std::wstring::size_type index = str.find(comp);
	if (index != std::wstring::npos && index == 0)
		return true;

	return false;
}

void ArkEngine::Utils::Replace(OUT std::string& str, std::string comp, std::string rep)
{
	std::string temp = str;

	size_t start_pos = 0;
	while ((start_pos = temp.find(comp, start_pos)) != std::string::npos)
	{
		temp.replace(start_pos, comp.length(), rep);
		start_pos += rep.length();
	}
}

void ArkEngine::Utils::Replace(OUT std::wstring& str, std::wstring comp, std::wstring rep)
{
	std::wstring temp = str;

	size_t start_pos = 0;
	while ((start_pos = temp.find(comp, start_pos)) != std::wstring::npos)
	{
		temp.replace(start_pos, comp.length(), rep);
		start_pos += rep.length();
	}
}

std::wstring ArkEngine::Utils::ToWString(std::string value)
{
	return std::wstring(value.begin(), value.end());
}

std::string ArkEngine::Utils::ToString(std::wstring value)
{
	return std::string(value.begin(),value.end());
}

//const char* ArkEngine::Utils::WstringToChar(const std::wstring& wstr)
//{
//	// wstring을 UTF-8로 변환합니다.
//	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
//	std::string utf8 = converter.to_bytes(wstr);
//
//	// const char*로 변환하여 반환합니다.
//	return utf8.c_str();
//}
