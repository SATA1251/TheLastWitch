/// <summary>
/// 그래픽스에 사용되는 큐브맵
/// 
/// 윤종화
/// </summary>


#pragma once

namespace ArkEngine
{
	class ICamera;

	class ICubeMap abstract
	{
	public:
		ICubeMap() {};
		virtual ~ICubeMap() {};

	public:
		virtual void Update(ArkEngine::ICamera* camera) abstract;
		virtual void Render() abstract;

		virtual const std::string& GetName() abstract;
		virtual const std::string& GetTextureName() abstract;
	};
}