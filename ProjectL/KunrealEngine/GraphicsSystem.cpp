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
	// ���콺 ��ǥ�� 0, 0 ���� Ŭ ��
	if (mouseX >= 0 && mouseY >= 0)
	{
		// ���콺 ��Ŭ�� ��
		if (InputSystem::GetInstance()->MouseButtonDown(0) && mouseX <= 1920 && mouseY <= 1080)
		{
			GInterface::GraphicsRenderable* picked = this->_graphics->GetPickedRenderable(mouseX, mouseY);

			if (picked != nullptr)
			{
				// �ش� renderable ��ü�� �ִ� �� Ž�� �� ��ġ�ϸ� ��ȯ
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
				// Ŭ���� ���� pick �����Ѱ� �ƴϸ� null��
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