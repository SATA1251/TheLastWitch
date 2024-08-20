/// <summary>
/// 
/// ����Ʈ�� ������ �Ŵ���
/// ������ �������� ����Ʈ���� �������̾���ϱ⿡
/// ���� �Ҵ����� �ʰ� ���ÿ� �Ҵ�
/// 
/// ����ȭ
/// </summary>

#pragma once
#include <vector>

namespace GInterface
{
	class GraphicsDirLight;
	class GraphicsPointLight;
}

namespace ArkEngine
{
	class IdirLight;
	class IPointLight;

	namespace ArkDX11
	{
		class DirectionalLight;
		class PointLight;
	}
}

namespace ArkEngine
{
	class LightManager
	{
	private:
		LightManager();
		~LightManager();

		static LightManager* instance;

	public:
		static LightManager* GetInstance();
		static void Delete();

	public:
		std::vector<ArkEngine::ArkDX11::DirectionalLight>& GetAllDirLightList();

		std::vector<ArkEngine::ArkDX11::DirectionalLight>& GetActiveDirLightList();

		std::vector<ArkEngine::IdirLight*>& GetIDirLightList();

		GInterface::GraphicsDirLight* GetDirLightInterface();

		std::vector<int>& GetActiveIndexList();

		void AddDirectionalLight(DirectX::XMFLOAT4 ambient, DirectX::XMFLOAT4 diffuse, DirectX::XMFLOAT4 specular, DirectX::XMFLOAT3 direction);		

	public:
		std::vector<ArkEngine::ArkDX11::PointLight>& GetAllPointLightList();
		
		std::vector<ArkEngine::ArkDX11::PointLight>& GetActivePointLightList();

		std::vector<ArkEngine::IPointLight*>& GetIPointLightList();

		GInterface::GraphicsPointLight* GetPointLightInterface();

		void AddPointLight(DirectX::XMFLOAT4 ambient, DirectX::XMFLOAT4 diffuse, DirectX::XMFLOAT4 specular, DirectX::XMFLOAT3 position, float range, float att);

	private:
		// directional Light
		std::vector<ArkEngine::ArkDX11::DirectionalLight> _dirLightList;

		std::vector<ArkEngine::ArkDX11::DirectionalLight> _activeDirLightList;

		std::vector<int> _activeDirIndexList;
	
		// point Light 
		std::vector<ArkEngine::ArkDX11::PointLight> _pointLightList;

		std::vector<ArkEngine::ArkDX11::PointLight> _activePointLightList;

	private:
		std::vector<ArkEngine::IdirLight*> _iDirLightList;

		std::vector<ArkEngine::IPointLight*> _iPointLightList;
	};
}