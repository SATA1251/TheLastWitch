#include <DirectXMath.h>
#include "IdirLight.h"
#include "IPointLight.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "LightManager.h"

ArkEngine::LightManager* ArkEngine::LightManager::instance = nullptr;

ArkEngine::LightManager::LightManager()
{

}

ArkEngine::LightManager::~LightManager()
{
	_activeDirLightList.clear();
	_iDirLightList.clear();
	_dirLightList.clear();
}

ArkEngine::LightManager* ArkEngine::LightManager::GetInstance()
{
	if (instance == nullptr)
	{
		instance = new ArkEngine::LightManager();
	}

	return instance;
}

void ArkEngine::LightManager::Delete()
{
	delete instance;
}

std::vector<ArkEngine::ArkDX11::DirectionalLight>& ArkEngine::LightManager::GetAllDirLightList()
{
	return _dirLightList;
}

std::vector<ArkEngine::ArkDX11::DirectionalLight>& ArkEngine::LightManager::GetActiveDirLightList()
{
	return _activeDirLightList;
}

std::vector<ArkEngine::IdirLight*>& ArkEngine::LightManager::GetIDirLightList()
{
	return _iDirLightList;
}

GInterface::GraphicsDirLight* ArkEngine::LightManager::GetDirLightInterface()
{
	return _iDirLightList.back();
}

std::vector<int>& ArkEngine::LightManager::GetActiveIndexList()
{
	return _activeDirIndexList;
}

void ArkEngine::LightManager::AddDirectionalLight(DirectX::XMFLOAT4 ambient, DirectX::XMFLOAT4 diffuse, DirectX::XMFLOAT4 specular, DirectX::XMFLOAT3 direction)
{
	_dirLightList.emplace_back(ArkEngine::ArkDX11::DirectionalLight(ambient, diffuse, specular, direction));

	unsigned int index = (unsigned int)_dirLightList.size()-1;
	_iDirLightList.emplace_back(new ArkEngine::IdirLight(index));
}

std::vector<ArkEngine::ArkDX11::PointLight>& ArkEngine::LightManager::GetAllPointLightList()
{
	return _pointLightList;
}

std::vector<ArkEngine::ArkDX11::PointLight>& ArkEngine::LightManager::GetActivePointLightList()
{
	return _activePointLightList;
}

std::vector<ArkEngine::IPointLight*>& ArkEngine::LightManager::GetIPointLightList()
{
	return _iPointLightList;
}

GInterface::GraphicsPointLight* ArkEngine::LightManager::GetPointLightInterface()
{
	return _iPointLightList.back();
}

void ArkEngine::LightManager::AddPointLight(DirectX::XMFLOAT4 ambient, DirectX::XMFLOAT4 diffuse, DirectX::XMFLOAT4 specular, DirectX::XMFLOAT3 position, float range, float attenuation)
{
	_pointLightList.emplace_back(ArkEngine::ArkDX11::PointLight(ambient, diffuse, specular, position, range/2, attenuation));
	
	unsigned index = (unsigned int)_pointLightList.size() - 1;
	_iPointLightList.emplace_back(new ArkEngine::IPointLight(index));
}