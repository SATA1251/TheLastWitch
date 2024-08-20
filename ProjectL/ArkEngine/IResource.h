/// <summary>
/// 그래픽스에 사용되는 다양한 리소스들이 상속받을 인터페이스
/// 리소스매니저와 연계시킬 예정
/// 
/// 윤종화
/// </summary>

#pragma once

namespace ArkEngine
{
	class IResource abstract
	{
	public:
		IResource() {};
		virtual ~IResource() {};

	public:
		virtual void Load(const char* fileName) abstract;
		virtual void UnLoad(const char* fileName) abstract;
	};
}