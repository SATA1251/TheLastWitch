#include "GraphicsSystem.h"
#include "InputSystem.h"
#include "MeshRenderer.h"

KunrealEngine::GraphicsSystem::GraphicsSystem()
	:_graphics(nullptr), _pickedObject(nullptr)
{

}

KunrealEngine::GraphicsSystem::~GraphicsSystem()
{

}

KunrealEngine::GraphicsSystem& KunrealEngine::GraphicsSystem::GetInstance()
{
	static GraphicsSystem _instance;
	return _instance;
}

void KunrealEngine::GraphicsSystem::Initialize(HWND hwnd, int screenWidth, int screenHeight)
{
	_graphics = CreateGraphicsEngine();
	_graphics->Initialize(reinterpret_cast<long long>(hwnd), screenWidth, screenHeight);
}

void KunrealEngine::GraphicsSystem::Update(int x, int y)
{
	SetPickedObject(x, y);
}

void KunrealEngine::GraphicsSystem::Release()
{
	ReleaseGraphicsEngine(_graphics);
}

GInterface::GraphicsInterface* KunrealEngine::GraphicsSystem::GetGraphics()
{
	return this->_graphics;
}

void KunrealEngine::GraphicsSystem::CreateCubeMap(const char* cubeMapName, const char* textureName /*= ""*/, bool isCube /*= false*/)
{
	return this->_graphics->CreateCubeMap(cubeMapName, textureName, isCube);
}

void KunrealEngine::GraphicsSystem::DeleteCubeMap(const char* cubeMapName)
{
	return this->_graphics->DeleteCubeMap(cubeMapName);
}

void KunrealEngine::GraphicsSystem::SetMainCubeMap(std::string cubeMapName)
{
	return this->_graphics->SetMainCubeMap(cubeMapName);
}

const std::vector<std::string> KunrealEngine::GraphicsSystem::GetCubeMapList()
{
	if (!_graphics->GetCubeMapList().empty())
	{
		return this->_graphics->GetCubeMapList();
	}
}

void KunrealEngine::GraphicsSystem::SetPickedObject(int mouseX, int mouseY)
{
	// 마우스 좌표가 0, 0 보다 클 때
	if (mouseX >= 0 && mouseY >= 0)
	{
		// 마우스 왼클릭 시
		if (InputSystem::GetInstance()->MouseButtonDown(0) && mouseX <= 1920 && mouseY <= 1080)
		{
			GInterface::GraphicsRenderable* picked = this->_graphics->GetPickedRenderable(mouseX, mouseY);

			if (picked != nullptr)
			{
				// 해당 renderable 객체가 있는 지 탐색 후 일치하면 반환
				for (auto obj : _pickableList)
				{
					if (obj->GetComponent<MeshRenderer>()->GetMeshObject() == this->_graphics->GetPickedRenderable(mouseX, mouseY))
					{
						this->_pickedObject = obj;
					}
				}
			}
			else
			{
				// 클릭한 것이 pick 가능한게 아니면 null로
				this->_pickedObject = nullptr;
			}
		}
	}
}

KunrealEngine::GameObject* KunrealEngine::GraphicsSystem::GetPickedObject()
{
	if (this->_pickedObject != nullptr)
	{
		return this->_pickedObject;
	}
	else
	{
		return nullptr;
	}
}

void KunrealEngine::GraphicsSystem::ClearPickedObject()
{
	this->_pickedObject = nullptr;
}

const std::vector<KunrealEngine::GameObject*> KunrealEngine::GraphicsSystem::GetPickableList()
{
	if (!_pickableList.empty())
	{
		return this->_pickableList;
	}
	else
	{
		return std::vector<GameObject*>();
	}
}

const std::vector<std::string> KunrealEngine::GraphicsSystem::GetRenderableList()
{
	return this->_graphics->GetRenderableNameList();
}

const std::vector<std::string> KunrealEngine::GraphicsSystem::GetTextureList()
{
	return this->_graphics->GetTextureNameList();
}