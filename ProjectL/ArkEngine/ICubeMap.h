/// <summary>
/// �׷��Ƚ��� ���Ǵ� ť���
/// 
/// ����ȭ
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