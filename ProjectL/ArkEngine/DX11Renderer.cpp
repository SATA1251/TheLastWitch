#include <d3d11.h>
#include <dxgi.h>
#include <wincodec.h>
#include <assert.h>
#include <windows.h>
#include <filesystem>
#include "ArkDevice.h"
#include "ArkEffect.h"
#include "ArkTexture.h"
#include "ASEParser.h"
#include "ResourceManager.h"
#include "ICamera.h"
#include "Camera.h"
#include "ILineObject.h"
#include "GraphicsRenderable.h"
#include "GraphicsDebug.h"
#include "DebugObject.h"
#include "IRenderable.h"
#include "ICubeMap.h"
#include "IdirLight.h"
#include "CubeMap.h"
#include "LightManager.h"
#include "GraphicsDirLight.h"
#include "FBXMesh.h"
#include "../FbxLoader/AssimpTool.h"
#include "deferredRenderer.h"
#include "deferredBuffer.h"
#include "ShadowBuffer.h"
#include "UIImage.h"
#include "ArkBuffer.h"
#include "DWFont.h"
#include "ILineObject.h"
#include "LineObject.h"
#include "MeshRenderer.h"
#include "ParticleSystem.h"
#include "TransparentMesh.h"
#include "../KunrealEngine/TimeManager.h"

#include "DX11Renderer.h"

static int testdef = 9;

static float testAngle = 0.0f;

GInterface::GraphicsInterface* CreateGraphicsEngine()
{
	return new ArkEngine::ArkDX11::DX11Renderer();
}

void ReleaseGraphicsEngine(GInterface::GraphicsInterface* instance)
{
	delete instance;
}

ArkEngine::ArkDX11::DX11Renderer::DX11Renderer()
	: _device(nullptr), _deviceContext(nullptr), _4xMsaaQuality(0), _enable4xMsaa(false),
	_swapChain(nullptr), _renderTargetView(nullptr), _depthStencilView(nullptr),
	_solidRS(nullptr), _wireRS(nullptr), _shadowRS(nullptr),
	_depthStencilState(nullptr), _depthStencilStateDisable(nullptr),
	_font(nullptr), _mainCamera(nullptr), _originMainCamera(nullptr), _mainCubeMap(nullptr),
	_clientWidth(0), _clientHeight(0), _shadowHeight(0), _shadowWidth(0),
	_isDebugMode(false), _renderingImageView(nullptr), _colorTexture(nullptr),
	_viewPort(), _shadowViewPort(), _particleCamera(nullptr), _depthStencilStateNoWrite(nullptr), _spriteBatch(nullptr)
{
	for (int i = 0; i < 4; i++)
	{
		_backGroundColor[i] = 1.0f;
	}
}

ArkEngine::ArkDX11::DX11Renderer::~DX11Renderer()
{
	Finalize();
}

void ArkEngine::ArkDX11::DX11Renderer::Initialize(long long hwnd, int clientWidth, int clientHeight)
{
	HRESULT hrCoInit = CoInitialize(nullptr);

	InitializeAssimpTool();

	InitializeWIC();

	_clientWidth = clientWidth;
	_clientHeight = clientHeight;

	CreateDevice();
	CheckQualification();
	CreateSwapChain(hwnd);

	CreateRenderTargetView();
	CreateDepthStencilView();

	SetViewportWithDefaultCamera();

	CreateRenderState();
	CreateDepthStecilState();
	CreateBlendState();

	SetResourceManager();

	CreateFont();
	CreateEffect();

	CreateTexture();

	// ��ó�� ������ �ϴ� ������� ������ ���⼭ ����
	auto shadowMapWidth = _clientWidth * 2;
	auto shadowMapHeight = _clientHeight * 2;


	_deferredRenderer = std::make_unique<ArkEngine::ArkDX11::DeferredRenderer>(_clientWidth, _clientHeight, shadowMapWidth, shadowMapHeight);
	CreateShadowViewPort(shadowMapWidth, shadowMapHeight);

	SetPickingTexture();

	auto particleCameraPos = DirectX::XMFLOAT3{ 0.0f, 5000.0f, -1.0f };
	auto particleCameraTarget = DirectX::XMFLOAT3{ 0.0f, -15.0f, 0.0f };
	DirectX::XMFLOAT3 worldUp = { 0.0f, 1.0f, 0.0f };

	ArkEngine::ArkDX11::Camera* newCamera = new ArkEngine::ArkDX11::Camera(particleCameraPos, particleCameraTarget, worldUp);
	ICamera* iCamera = newCamera;

	iCamera->SetProjectionMatrix(0.25f * DirectX::XM_PI, GetAspectRatio(), 1.0f, 1000.0f, true);

	_particleCamera = iCamera;

	_spriteBatch = new DirectX::SpriteBatch(_deviceContext.Get());
}

void ArkEngine::ArkDX11::DX11Renderer::Initialize(long long hwnd, int clientWidth, int clientHeight, float backGroundColor[4])
{
	Initialize(hwnd, clientWidth, clientHeight);

	for (int i = 0; i < 4; i++)
	{
		_backGroundColor[i] = backGroundColor[i];
	}
}

void ArkEngine::ArkDX11::DX11Renderer::Update()
{
	_mainCamera->Update();

	if (_mainCubeMap != nullptr)
	{
		_mainCubeMap->Update(_mainCamera);
	}

	// Frustum culling�� ���� �׸��� �ʾƵ� ������Ʈ ��Ű���� ����
	for (const auto& index : ResourceManager::GetInstance()->GetDebugObjectList())
	{
		{
			index->Update(_mainCamera);
		}
	}

	for (const auto& index : ResourceManager::GetInstance()->GetRenderableList())
	{
		// �ϴ��� �׷��� �͸� ������Ʈ
		if (index->GetRenderingState() && index->GetInsideFrustumState())
		{
			index->Update(_mainCamera);
		}
	}

	ResourceManager::GetInstance()->SortMeshRendererByAlpha();

	//if (GetAsyncKeyState(0x23) & 0x0001)
	//{
	//	if (_isDebugMode == true)
	//	{
	//		_isDebugMode = false;
	//	}
	//	else
	//	{
	//		_isDebugMode = true;
	//	}
	//}

	// particle
	//float time = 0.032f;
	
	//float timaplus = 0.0f;
	//timaplus += time;
	//float gameTime = 0.0f;
	//gameTime += time;

	for (const auto& index : ResourceManager::GetInstance()->GetParticleList())
	{
		float speed = 1.8f;
		index->Update(_deltaT * speed, _deltaT * speed);
	}

	for (const auto& index : ResourceManager::GetInstance()->GetTransParentMeshList())
	{
		index->Update(_mainCamera);
	}


	// ����׿�
	if (_isDebugMode)
	{
		for (const auto& index : ResourceManager::GetInstance()->GetLineObjectList())
		{
			index->Update(_mainCamera);
		}

		if (GetAsyncKeyState(VK_F1))
		{
			testdef = 0;
		}

		if (GetAsyncKeyState(VK_F2))
		{
			testdef = 1;
		}

		if (GetAsyncKeyState(VK_F3))
		{
			testdef = 2;
		}

		if (GetAsyncKeyState(VK_F4))
		{
			testdef = 3;
		}

		if (GetAsyncKeyState(VK_F5))
		{
			testdef = 4;
		}

		if (GetAsyncKeyState(VK_F6))
		{
			testdef = 5;
		}
		if (GetAsyncKeyState(VK_F7))
		{
			testdef = 6;
		}
		if (GetAsyncKeyState(VK_F8))
		{
			testdef = 7;
		}
		if (GetAsyncKeyState(VK_F9))
		{
			testdef = 9;
		}
		if (GetAsyncKeyState(VK_F10))
		{
			testdef = 10;
		}
		if (GetAsyncKeyState('V'))
		{
			testdef = 20;
		}
		if (GetAsyncKeyState('B'))
		{
			testdef = 11;
		}
		if (GetAsyncKeyState('N'))
		{
			testdef = 12;
		}
		if (GetAsyncKeyState('M'))
		{
			testdef = 13;
		}

		if (GetAsyncKeyState('H') & 0x8000)
		{
			for (const auto& index : ResourceManager::GetInstance()->GetParticleList())
			{
				index->Stop();
			}
		}
		if (GetAsyncKeyState('J') & 0x8000)
		{
			for (const auto& index : ResourceManager::GetInstance()->GetParticleList())
			{
				index->Start();
			}
		}
	}
}

void ArkEngine::ArkDX11::DX11Renderer::Render()
{
	_decalWorldVec.clear();

	int testCullingNum = 0;

	// ������ ��ġ�� ī�޶�� ����
	const auto& lightIndexList = LightManager::GetInstance()->GetActiveIndexList();

	// ���� �ִٸ�
	if (lightIndexList.empty() == false)
	{
		// Perspective Shadow�� �����ϱ� ���� ī�޶� ���� �������� ���������� �������� �ٲ�
		_mainCamera = ResourceManager::GetInstance()->GetShadowCamera()[0];

		// ������ ���ۿ� �������������� ��ü���� ���̰��� ����� �غ� 
		BeginShadowRender();

		for (const auto& index : ResourceManager::GetInstance()->GetAllMeshRenderer())
		{
			index->SetMainCamera(_mainCamera);
			index->ShadowRender();
		}

		// ���� �������� ���� ���������� ī�޶�� �ǵ���
		_mainCamera = _originMainCamera;
	}

	// ���۵� ���ۿ� ���� ī�޶� ���������� ������Ʈ���� ������ �ϱ� ���� �غ�
	BeginRender();

	BeginTransparentSet();

	for (const auto& index : ResourceManager::GetInstance()->GetAllNoTransParentRenderer())
	{
		index->SetMainCamera(_mainCamera);
		index->Render();
	}

	BeginTransparentSet();

	ResourceManager::GetInstance()->SortTransParentMesh();

	_deviceContext->OMSetDepthStencilState(_depthStencilStateNoWrite.Get(), 0);

	ResourceManager::GetInstance()->ClearDecalMeshVec();

	for (const auto& index : ResourceManager::GetInstance()->GetTransParentMeshList())
	{
		index->Render(_decalWorldVec);
	}

	//_deviceContext->OMSetDepthStencilState(_depthStencilState.Get(), 0);
	//
	//for (const auto& index : ResourceManager::GetInstance()->GetAllTransParentRenderer())
	//{
	//	index->SetMainCamera(_mainCamera);
	//	index->Render();
	//}

	_deviceContext->OMSetDepthStencilState(_depthStencilState.Get(), 0);

	EndTransparentSet();

	//// UI, FONT ����� ���� ���� �����ִ� ���� ���� ����
	_deviceContext->OMSetDepthStencilState(_depthStencilStateDisable.Get(), 0);

	BeginFinalRender();

	BeginTransparentSet();

	// UI, FONT ����� ���� ���� �����ִ� ���� ���� ����
	_deviceContext->OMSetDepthStencilState(_depthStencilStateDisable.Get(), 0);

	_deferredRenderer->RenderForFinalTexture(_decalWorldVec);

	//_deviceContext->OMSetDepthStencilState(_depthStencilState.Get(), 0);
	//
	//ResourceManager::GetInstance()->SortTransParentMesh();
	//
	//for (auto& index : ResourceManager::GetInstance()->GetTransParentMeshList())
	//{
	//	index->Render(_deferredRenderer->GetDeferredBuffer());
	//}
	//
	//_deviceContext->OMSetDepthStencilState(_depthStencilStateDisable.Get(), 0);

	auto bloomCount = _deferredRenderer->GetDeferredBuffer()->GetSRVForBloomVec().size();
	for (int i = 0; i < bloomCount; i++)
	{
		BeginBloomRender(i);

		_deferredRenderer->RenderForBloom(i);
	}


	// ���������� ���۵� ���۸� �ջ��� ������� ȭ�鿡 ����� �غ�
	FinalRender();

	BeginTransparentSet();

	// ���۵� ���۸� �����Ͽ� �� Texture2D�� �ϼ��� ȭ���� �����
	_deferredRenderer->Update(_mainCamera);
	_deferredRenderer->Render();

	_deviceContext->OMSetDepthStencilState(_depthStencilStateDisable.Get(), 0);

	EndTransparentSet();

	for (const auto& index : ResourceManager::GetInstance()->GetParticleList())
	{
		index->Draw(_mainCamera, _particleCamera, 1);
	}

	// ť��� ������
	if (_mainCubeMap != nullptr)
	{
		_mainCubeMap->Render();
	}

	BeginTransparentSet();

	_deviceContext->OMSetDepthStencilState(_depthStencilState.Get(), 0);

	for (const auto& index : ResourceManager::GetInstance()->GetAllTransParentRenderer())
	{
		index->SetMainCamera(_mainCamera);
		index->RenderForward();
	}

	EndTransparentSet();

	_font->RenderUI();


	// ����� ����� ���
	if (_isDebugMode)
	{
		// �������� �ø���, �޽� ũ�⸸ŭ�� ����׿� ������Ʈ ������
		for (const auto& index : ResourceManager::GetInstance()->GetDebugObjectList())
		{
			if (index->GetRenderingState() && index->GetActive())
			{
				if (index->GetName() != "navMesh")
				{
					index->Render();
				}
			}
		}

		// ���� ������Ʈ ������
		for (const auto& index : ResourceManager::GetInstance()->GetLineObjectList())
		{
			index->Render();
		}

		// DrawDebugText�� ���� ������ ��� ����׿� ��Ʈ ������
		_deviceContext->OMSetDepthStencilState(_depthStencilStateDisable.Get(), 0);

		_font->RenderDebug();
	}

	// ����׿������� �ϴ� �и�
	{
		// �������� �ø���, �޽� ũ�⸸ŭ�� ����׿� ������Ʈ ������
		for (const auto& index : ResourceManager::GetInstance()->GetDebugObjectList())
		{
			if (index->GetName() == "navMesh")
			{
				if (index->GetActive())
				{
					index->Render();
				}
			}
		}
	}

	_spriteBatch->Begin();

	// UI IMAGE ������
	for (const auto& index : ResourceManager::GetInstance()->GetUIImageList())
	{
		if (index->GetRenderingState())
		{
			// 0�� �н��� �������� UI IMAGE�� ������
			index->Render(_spriteBatch);
		}
	}

	_spriteBatch->End();

	ID3D11ShaderResourceView* srv = NULL;

	for (int i = 0; i < static_cast<int>(eBUFFERTYPE::GBUFFER_COUNT); i++)
	{
		_deviceContext->PSSetShaderResources(i, 1, &srv);
	}

	EndRender();
}

void ArkEngine::ArkDX11::DX11Renderer::Finalize()
{
	_colorTexture->Release();

	_deferredRenderer->Finalize();

	if (_renderingImageView != nullptr)
	{
		_renderingImageView->Release();
	}

	delete _spriteBatch;

	ResourceManager::GetInstance()->ReleaseAll();

	ArkEngine::ResourceManager::GetInstance()->Delete();

	// �̰� �ϸ� warning�� �پ����ϴµ�.. (ref, intref�� 0�� �͵��� �����)
	//_deviceContext->ClearState();
	//_deviceContext->Flush();

	ID3D11Debug* debugDevice = nullptr;
	HRESULT hr = _device->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&debugDevice));
	if (SUCCEEDED(hr)) {
		debugDevice->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
		debugDevice->Release();
	}
}

GInterface::GraphicsRenderable* ArkEngine::ArkDX11::DX11Renderer::CreateRenderable(const char* fileName, bool isSold)
{
	std::string tempString = fileName;

	ArkEngine::IRenderable* newObject = nullptr;

	//if (tempString.find("ASEFile") != std::string::npos)
	//{
	//	newObject = new ASEMesh(fileName);
	//}
	//else
	{
		newObject = new FBXMesh(fileName, isSold);
	}

	ResourceManager::GetInstance()->AddRenderable(newObject);

	GInterface::GraphicsRenderable* tempObject = dynamic_cast<GInterface::GraphicsRenderable*>(newObject);

	return tempObject;
}

void ArkEngine::ArkDX11::DX11Renderer::DeleteRenderable(GInterface::GraphicsRenderable* renderable)
{
	ArkEngine::IRenderable* tempObject = dynamic_cast<ArkEngine::IRenderable*>(renderable);

	auto meshRenderer = ResourceManager::GetInstance()->GetMeshRenderer(tempObject->GetName());

	meshRenderer->DeleteMeshInList(tempObject);

	renderable->Delete();
}

GInterface::GraphicsRenderable* ArkEngine::ArkDX11::DX11Renderer::GetPickedRenderable(int mouseX, int mouseY)
{
	auto pickingID = Picking(mouseX, mouseY);

	const auto& renderableList = ResourceManager::GetInstance()->GetRenderableList();

	for (const auto& index : renderableList)
	{
		if (index->GetPickable() == true)
		{
			if (pickingID == index->GetHashID())
			{
				GInterface::GraphicsRenderable* tempObject = dynamic_cast<GInterface::GraphicsRenderable*>(index);
				return tempObject;
			}
		}
	}

	return nullptr;
}

GInterface::GraphicsDebug* ArkEngine::ArkDX11::DX11Renderer::CreateDebugCube(const char* objectName, float width, float height, float depth)
{
	DirectX::XMFLOAT4 color = DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	GInterface::GraphicsDebug* debugObject = new DebugObject(objectName, width, height, depth, color);

	return debugObject;
}

GInterface::GraphicsDebug* ArkEngine::ArkDX11::DX11Renderer::CreateDebugSphere(const char* objectName, float radius)
{
	float center = radius / 2;
	DirectX::XMFLOAT4 color = DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

	GInterface::GraphicsDebug* debugObject = new DebugObject(objectName, center, radius, color);

	return debugObject;
}


GInterface::GraphicsDebug* ArkEngine::ArkDX11::DX11Renderer::CreateMapDebug(const char* objectName, std::vector<DirectX::XMFLOAT3>& vertexVec, std::vector<unsigned int>& indexVec)
{
	GInterface::GraphicsDebug* debugObject = new DebugObject(objectName, vertexVec, indexVec);

	return debugObject;
}

void ArkEngine::ArkDX11::DX11Renderer::DeleteDebugObject(GInterface::GraphicsDebug* debugObject)
{
	delete debugObject;
}


void ArkEngine::ArkDX11::DX11Renderer::DeleteDebugMap(const std::string& name)
{
	auto vec = ResourceManager::GetInstance()->GetDebugObjectList();
	for (int i = 0; i < vec.size(); i++)
	{
		if (vec[i]->GetName() == name)
		{
			vec[i]->ReleaseWithBuffer();

			vec.erase(vec.begin() + i);

			return;
		}
	}
}

GInterface::GraphicsDebug* ArkEngine::ArkDX11::DX11Renderer::GetDebugObject(const std::string& name)
{
	for (auto index : ResourceManager::GetInstance()->GetDebugObjectList())
	{
		if (index->GetName() == name)
		{
			return static_cast<DebugObject*> (index);
		}
	}
}

void ArkEngine::ArkDX11::DX11Renderer::CreateDebugLine(const DirectX::XMFLOAT3& vertex1, const DirectX::XMFLOAT3& vertex2, const DirectX::XMFLOAT4& color)
{
	auto lineList = ResourceManager::GetInstance()->GetLineObjectList();

	for (const auto& index : lineList)
	{
		if (Equal(index->GetVertex1(), vertex1))
		{
			if (Equal(index->GetVertex2(), vertex2))
			{
				return;
			}
		}

		if (Equal(index->GetVertex2(), vertex1))
		{
			if (Equal(index->GetVertex1(), vertex2))
			{
				return;
			}
		}
	}

	ILineObject* object = new LineObject();
	object->Initialize(vertex1, vertex2, color);

	ResourceManager::GetInstance()->AddLineObject(object);
}

void ArkEngine::ArkDX11::DX11Renderer::DeleteLine(int index)
{
	if (ResourceManager::GetInstance()->GetLineObjectList().size() < index)
	{
		auto object = ResourceManager::GetInstance()->GetLineObjectList()[index];
		ResourceManager::GetInstance()->DeleteLineObject(object);
	}
}

void ArkEngine::ArkDX11::DX11Renderer::DeleteLine(const DirectX::XMFLOAT3& vertex1, const DirectX::XMFLOAT3& vertex2)
{
	auto lineList = ResourceManager::GetInstance()->GetLineObjectList();

	for (const auto& index : lineList)
	{
		if (Equal(index->GetVertex1(), vertex1))
		{
			if (Equal(index->GetVertex2(), vertex2))
			{
				ResourceManager::GetInstance()->DeleteLineObject(index);
				return;
			}
		}

		if (Equal(index->GetVertex2(), vertex1))
		{
			if (Equal(index->GetVertex1(), vertex2))
			{
				ResourceManager::GetInstance()->DeleteLineObject(index);
				return;
			}
		}
	}
}

void ArkEngine::ArkDX11::DX11Renderer::DeleteAllLine()
{
	for (auto& index : ResourceManager::GetInstance()->GetLineObjectList())
	{
		ResourceManager::GetInstance()->DeleteLineObject(index);
	}
}

void ArkEngine::ArkDX11::DX11Renderer::CreateCubeMap(const char* cubeMapName, const char* textureName, bool isCube)
{
	ResourceManager::GetInstance()->AddCubeMap(new CubeMap(cubeMapName, textureName, isCube));
}

void ArkEngine::ArkDX11::DX11Renderer::DeleteCubeMap(const char* cubeMapName)
{
	auto MapList = ResourceManager::GetInstance()->GetCubeMapList();

	for (const auto& index : MapList)
	{
		if (index->GetName() == cubeMapName)
		{
			ResourceManager::GetInstance()->DeleteCubeMap(index);
			break;
		}
	}
}

const std::vector<std::string>& ArkEngine::ArkDX11::DX11Renderer::GetCubeMapList()
{
	return ResourceManager::GetInstance()->GetCubeMapNameList();
}

void ArkEngine::ArkDX11::DX11Renderer::SetMainCubeMap(std::string cubeMapName)
{
	auto mapList = ResourceManager::GetInstance()->GetCubeMapList();

	for (const auto& index : mapList)
	{
		if (index->GetName() == cubeMapName)
		{
			ResourceManager::GetInstance()->SetNowCubeMap(index);
			_mainCubeMap = index;

			break;
		}
	}
}

GInterface::GraphicsImage* ArkEngine::ArkDX11::DX11Renderer::CreateImage(const char* imageName)
{
	GInterface::GraphicsImage* image = new UIImage(imageName, _clientWidth, _clientHeight);

	return image;
}

void ArkEngine::ArkDX11::DX11Renderer::DeleteImage(GInterface::GraphicsImage* image)
{
	image->Delete();
}

GInterface::GraphicsImage* ArkEngine::ArkDX11::DX11Renderer::GetPickedImage(int mouseX, int mouseY)
{
	auto pickingID = Picking(mouseX, mouseY);

	auto uiList = ResourceManager::GetInstance()->GetUIImageList();

	for (const auto& index : uiList)
	{
		//if (pickingID == index->GetHashID())
		{
			GInterface::GraphicsImage* tempImage = dynamic_cast<GInterface::GraphicsImage*>(index);
			return tempImage;
		}
	}

	return nullptr;
}

GInterface::GraphicsCamera* ArkEngine::ArkDX11::DX11Renderer::CreateCamera(const DirectX::XMFLOAT3& cameraPosition, const DirectX::XMFLOAT3& targetPosition)
{
	DirectX::XMFLOAT3 worldUp = { 0.0f, 1.0f, 0.0f };

	ArkEngine::ArkDX11::Camera* newCamera = new ArkEngine::ArkDX11::Camera(cameraPosition, targetPosition, worldUp);
	ICamera* iCamera = newCamera;

	iCamera->SetProjectionMatrix(0.25f * DirectX::XM_PI, GetAspectRatio(), 1.0f, 1000.0f, true);

	ResourceManager::GetInstance()->AddCamera(iCamera);

	GInterface::GraphicsCamera* graphicsCamera = newCamera;

	return graphicsCamera;
}

void ArkEngine::ArkDX11::DX11Renderer::DeleteCamera(GInterface::GraphicsCamera* camera)
{
	camera->Delete();
}

GInterface::GraphicsCamera* ArkEngine::ArkDX11::DX11Renderer::GetMainCamera()
{
	auto camera = dynamic_cast<ArkEngine::ArkDX11::Camera*> (_mainCamera);
	GInterface::GraphicsCamera* mainCamera = camera;

	return mainCamera;
}

void ArkEngine::ArkDX11::DX11Renderer::SetMainCamera(GInterface::GraphicsCamera* camera)
{
	for (const auto& index : ResourceManager::GetInstance()->GetCameraList())
	{
		if (index->GetMain() == true)
		{
			index->SetMain(false);
			break;
		}
	}

	_mainCamera = dynamic_cast<ArkEngine::ArkDX11::Camera*>(camera);

	_mainCamera->SetMain(true);
	_originMainCamera = _mainCamera;
}

GInterface::GraphicsDirLight* ArkEngine::ArkDX11::DX11Renderer::CreateDirectionalLight(const DirectX::XMFLOAT4& ambient, const DirectX::XMFLOAT4& diffuse, const DirectX::XMFLOAT4& specular, const DirectX::XMFLOAT3& direction)
{
	DirectX::XMFLOAT4 amb = ambient;
	DirectX::XMFLOAT4 dif = diffuse;
	DirectX::XMFLOAT4 spec = specular;
	DirectX::XMFLOAT3 dir = direction;

	CreateDirLight(amb, dif, spec, dir);

	// ���� �ϳ��� Directional Light�� ���ؼ��� �׸��� ó���� �����ϹǷ�
	// ó�� ���� �𷺼ų� ����Ʈ�� ���� ī�޶� ����
	if (ResourceManager::GetInstance()->GetShadowCamera().empty())
	{
		DirectX::XMFLOAT3 targetPos = { 0.0f, 0.0f, 0.0f };
		CreateShadowCamera(GetMyPosition(direction, targetPos), targetPos);
	}
	else
	{
		auto iCamera = ResourceManager::GetInstance()->GetShadowCamera().back();

		DirectX::XMFLOAT3 targetPos = { 0.0f, 0.0f, 0.0f };
		iCamera->SetCameraPos(targetPos, direction, 200.0f);
	}


	return LightManager::GetInstance()->GetDirLightInterface();
}

GInterface::GraphicsPointLight* ArkEngine::ArkDX11::DX11Renderer::CreatePointLight(const DirectX::XMFLOAT4& ambient, const DirectX::XMFLOAT4& diffuse, const DirectX::XMFLOAT4& specular, const DirectX::XMFLOAT3& position, float range, float attenuation)
{
	DirectX::XMFLOAT4 amb = ambient;
	DirectX::XMFLOAT4 dif = diffuse;
	DirectX::XMFLOAT4 spec = specular;
	DirectX::XMFLOAT3 pos = position;

	CreatePoLight(amb, dif, spec, pos, range, attenuation);

	return LightManager::GetInstance()->GetPointLightInterface();
}


GInterface::GraphicsParticle* ArkEngine::ArkDX11::DX11Renderer::CreateParticle(const std::string& particleName, const std::string& fileName, unsigned int maxParticle)
{
	auto particle = new ArkEngine::ParticleSystem(particleName, fileName, maxParticle);

	GInterface::GraphicsParticle* pParticle = particle;

	ResourceManager::GetInstance()->AddParticle(particle);

	return pParticle;
}


void ArkEngine::ArkDX11::DX11Renderer::DeleteParticle(GInterface::GraphicsParticle* particle)
{

}

void ArkEngine::ArkDX11::DX11Renderer::SetPickingTexture()
{
	D3D11_TEXTURE2D_DESC texCopyDesc;
	ZeroMemory(&texCopyDesc, sizeof(texCopyDesc));
	texCopyDesc.Width = 1;
	texCopyDesc.Height = 1;
	texCopyDesc.MipLevels = 1;
	texCopyDesc.ArraySize = 1;
	texCopyDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texCopyDesc.SampleDesc.Count = 1;
	texCopyDesc.SampleDesc.Quality = 0;
	texCopyDesc.Usage = D3D11_USAGE_STAGING;
	texCopyDesc.BindFlags = 0;
	texCopyDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
	texCopyDesc.MiscFlags = 0;

	_device->CreateTexture2D(&texCopyDesc, NULL, &_colorTexture);
}

UINT ArkEngine::ArkDX11::DX11Renderer::Picking(int mouseX, int mouseY)
{
	D3D11_BOX newBox;
	newBox.left = mouseX;
	newBox.right = mouseX + 1;
	newBox.top = mouseY;
	newBox.bottom = mouseY + 1;
	newBox.front = 0;
	newBox.back = 1;

	auto originColorTexture = _deferredRenderer->GetDeferredBuffer()->GetTextrue(static_cast<int>(eBUFFERTYPE::GBUFFER_COLOR));

	_deviceContext->CopySubresourceRegion(_colorTexture, 0, 0, 0, 0, originColorTexture, 0, &newBox);

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	_deviceContext->Map(_colorTexture, 0, D3D11_MAP_READ, 0, &mappedResource);

	if (mappedResource.pData != nullptr)
	{
		auto objectID = ((UINT*)mappedResource.pData)[0];

		_deviceContext->Unmap(_colorTexture, 0);

		return objectID;
	}
	else return 0;
}

const DirectX::XMFLOAT3 ArkEngine::ArkDX11::DX11Renderer::GetMyPosition(const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT3& targetPos)
{
	DirectX::XMVECTOR myPos = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&targetPos), DirectX::XMLoadFloat3(&direction));

	float scaleFactor = 500.0f;

	auto newVec = DirectX::XMVectorScale(myPos, scaleFactor);

	DirectX::XMFLOAT3 myPosition;
	DirectX::XMStoreFloat3(&myPosition, newVec);

	return myPosition;
}

void ArkEngine::ArkDX11::DX11Renderer::SetDeltaTime(float dt)
{
	_deltaT = dt;
}

void ArkEngine::ArkDX11::DX11Renderer::BeginShadowRender()
{
	SetShadowViewport();

	_deferredRenderer->BeginShadowRender();

	_deviceContext->OMSetDepthStencilState(_depthStencilState.Get(), 0);
}

void ArkEngine::ArkDX11::DX11Renderer::BeginRender()
{
	SetViewport();

	_deferredRenderer->BeginRender();

	_deviceContext->ClearDepthStencilView(_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0F, 0);

	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	UINT sampleMask = 0xffffffff;
	_deviceContext->OMSetBlendState(_blendState.Get(), blendFactor, sampleMask);

	_deviceContext->OMSetDepthStencilState(_depthStencilState.Get(), 0);
}


void ArkEngine::ArkDX11::DX11Renderer::BeginFinalRender()
{
	_deferredRenderer->BeginFinalRender();

	_deviceContext->OMSetDepthStencilState(_depthStencilStateDisable.Get(), 0);
}

void ArkEngine::ArkDX11::DX11Renderer::BeginBloomRender(int index)
{
	_deferredRenderer->BeginBloomRender(index);

	_deviceContext->OMSetDepthStencilState(_depthStencilStateDisable.Get(), 0);
}

void ArkEngine::ArkDX11::DX11Renderer::FinalRender()
{
	BindView();

	_deviceContext->ClearRenderTargetView(_renderTargetView.Get(), _backGroundColor);

	_deviceContext->OMSetDepthStencilState(_depthStencilStateDisable.Get(), 0);
}

void ArkEngine::ArkDX11::DX11Renderer::EndRender()
{
	// Present (������� �ֻ���, ����ȭ ��� -> 0�� �⺻, ǥ�ÿ� ���� �÷��׳� �߰� �ɼ� -> 0�� �⺻)
	_swapChain->Present(0, 0);
}

void ArkEngine::ArkDX11::DX11Renderer::BeginTransparentSet()
{
	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	UINT sampleMask = 0xffffffff;
	_deviceContext->OMSetBlendState(_blendState.Get(), blendFactor, sampleMask);
}

void ArkEngine::ArkDX11::DX11Renderer::EndTransparentSet()
{
	// ������ ���ؽ�Ʈ�� ���� ������Ʈ ����
	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	UINT sampleMask = 0xffffffff;

	// ���� ����
	_deviceContext->OMSetBlendState(nullptr, blendFactor, sampleMask);

	_deviceContext->OMSetDepthStencilState(_depthStencilStateDisable.Get(), 0);
}

void ArkEngine::ArkDX11::DX11Renderer::DrawDebugText(int posX, int posY, int fontSize, const char* text, ...)
{
	va_list vl;
	// text�� ��ġ�� ������� �������ڸ� ����
	va_start(vl, text);

	// �������ڸ� �Ű������� �Ѱ��� �� ���⿡ ���ڿ� �迭�� ��Ƽ� ����
	char buffer[256];
	vsnprintf(buffer, sizeof(buffer), text, vl);

	std::string result(buffer);

	// �ѱ� ���ڿ��� ���
	_font->RenderDebugText(posX, posY, result, fontSize);

	va_end(vl);

}

void ArkEngine::ArkDX11::DX11Renderer::DrawUIText(int posX, int posY, int fontSize, const DirectX::XMFLOAT4& color, const char* text, ...)
{
	va_list vl;
	// text�� ��ġ�� ������� �������ڸ� ����
	va_start(vl, text);

	// �������ڸ� �Ű������� �Ѱ��� �� ���⿡ ���ڿ� �迭�� ��Ƽ� ����
	char buffer[256];
	vsnprintf(buffer, sizeof(buffer), text, vl);

	std::string result(buffer);

	// �ѱ� ���ڿ��� ���
	_font->RenderUIText(posX, posY, result, fontSize, color);

	va_end(vl);
}

void* ArkEngine::ArkDX11::DX11Renderer::GetRenderingImage()
{
	ID3D11Texture2D* renderingImage = nullptr;
	ID3D11Texture2D* backBuffer = nullptr;

	HRESULT hr = _swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**> (&backBuffer));

	D3D11_TEXTURE2D_DESC desc;
	backBuffer->GetDesc(&desc);
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;

	_device->CreateTexture2D(&desc, NULL, &renderingImage);

	_deviceContext->CopyResource(renderingImage, backBuffer);

	if (_renderingImageView != nullptr)
	{
		_renderingImageView->Release();
		_renderingImageView = nullptr;
	}
	_device->CreateShaderResourceView(renderingImage, nullptr, &_renderingImageView);

	if (_renderingImageView == nullptr)
	{
		return nullptr;
	}

	backBuffer->Release();
	renderingImage->Release();

	if (testdef == 20)
	{
		return static_cast<void*> (_deferredRenderer->GetDeferredBuffer()->GetSRV(7));
	}
	else if (testdef == 9)
	{
		return static_cast<void*> (_renderingImageView);
	}
	else if (testdef == 7)
	{
		return static_cast<void*> (_deferredRenderer->GetShadowBuffer()->GetDepthSRV());
	}
	else if (testdef < static_cast<int>(eBUFFERTYPE::GBUFFER_COUNT))
	{
		return static_cast<void*> (_deferredRenderer->GetDeferredBuffer()->GetSRV(testdef));
	}
	else if (testdef == 10)
	{
		return static_cast<void*> (_deferredRenderer->GetDeferredBuffer()->GetSRVForFinal(0));
	}
	else if (testdef > 10)
	{
		return static_cast<void*> (_deferredRenderer->GetDeferredBuffer()->GetSRVForBloom(testdef - 11));
	}
}

void* ArkEngine::ArkDX11::DX11Renderer::GetDevice()
{
	auto device = ResourceManager::GetInstance()->GetResource<ArkEngine::ArkDX11::ArkDevice>("Device")->GetDevice();

	return static_cast<void*> (device);
}

void* ArkEngine::ArkDX11::DX11Renderer::GetDeviceContext()
{
	auto deviceContext = ResourceManager::GetInstance()->GetResource<ArkEngine::ArkDX11::ArkDevice>("Device")->GetDeviceContext();

	return static_cast<void*> (deviceContext);
}

void* ArkEngine::ArkDX11::DX11Renderer::GetRenderTargetView()
{
	auto renderTargetView = _renderTargetView.Get();

	return static_cast<void*>(renderTargetView);
}

const std::vector<std::string> ArkEngine::ArkDX11::DX11Renderer::GetRenderableNameList()
{
	auto nameVector = ResourceManager::GetInstance()->GetRenderableNameList();
	auto fbxNameVector = _assimpTool->GetSkinnedMeshName();

	nameVector.insert(nameVector.end(), fbxNameVector.begin(), fbxNameVector.end());

	return nameVector;
}

const std::vector<std::string>& ArkEngine::ArkDX11::DX11Renderer::GetTextureNameList()
{
	return ResourceManager::GetInstance()->GetTextureNameList();
}


const DirectX::XMFLOAT3 ArkEngine::ArkDX11::DX11Renderer::ScreenToWorldPoint(int mouseX, int mouseY)
{
	// screen ��ǥ�踦 ndc�� ��ȯ
	float normalizedX = ((float)mouseX / (float)_clientWidth) * 2 - 1;
	float normalizedY = 1 - ((float)mouseY / (float)_clientHeight) * 2;

	// ndc�� proj ������� ���� view �������� ��ȯ
	float viewX = normalizedX / _mainCamera->GetProjMatrix().r[0].m128_f32[0];
	float viewY = normalizedY / _mainCamera->GetProjMatrix().r[1].m128_f32[1];

	DirectX::XMVECTOR rayDir = DirectX::XMVectorSet(viewX, viewY, 1.0f, 1.0f);
	rayDir = DirectX::XMVector3Normalize(rayDir);

	DirectX::XMMATRIX invView = DirectX::XMMatrixInverse(nullptr, _mainCamera->GetViewMatrix());

	DirectX::XMVECTOR rayOrigin = DirectX::XMVectorSet(_mainCamera->GetCameraPos().x, _mainCamera->GetCameraPos().y, _mainCamera->GetCameraPos().z, 1.0f);

	// view ������ view ����� ������� ���� world �������� ��ȯ
	DirectX::XMVECTOR rayDirToWorld = DirectX::XMVector3TransformCoord(rayDir, invView);

	DirectX::XMVECTOR intersectionPoint = DirectX::XMPlaneIntersectLine(
		DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f),        // ����� ������ ���
		rayOrigin,		// ������ ������ (ī�޶� ��ġ)
		rayDirToWorld   // ������ ����
	);

	return DirectX::XMFLOAT3(intersectionPoint.m128_f32[0], intersectionPoint.m128_f32[1], intersectionPoint.m128_f32[2]);
}

const DirectX::XMFLOAT3 ArkEngine::ArkDX11::DX11Renderer::ScreenToWorldPoint(int mouseX, int mouseY, float planeD)
{
	// Screen coordinates to NDC
	float normalizedX = ((float)mouseX / (float)_clientWidth) * 2 - 1;
	float normalizedY = 1 - ((float)mouseY / (float)_clientHeight) * 2;

	// NDC to view space
	float viewX = normalizedX / _mainCamera->GetProjMatrix().r[0].m128_f32[0];
	float viewY = normalizedY / _mainCamera->GetProjMatrix().r[1].m128_f32[1];

	DirectX::XMVECTOR rayDir = DirectX::XMVectorSet(viewX, viewY, 1.0f, 0.0f);
	rayDir = DirectX::XMVector3Normalize(rayDir);

	DirectX::XMMATRIX invView = DirectX::XMMatrixInverse(nullptr, _mainCamera->GetViewMatrix());

	DirectX::XMVECTOR rayOrigin = DirectX::XMVectorSet(_mainCamera->GetCameraPos().x, _mainCamera->GetCameraPos().y, _mainCamera->GetCameraPos().z, 1.0f);

	// Transform ray direction from view space to world space
	DirectX::XMVECTOR rayDirToWorld = DirectX::XMVector3TransformNormal(rayDir, invView);
	rayDirToWorld = DirectX::XMVector3Normalize(rayDirToWorld);

	// Define the plane equation (y = 66)
	DirectX::XMVECTOR planeNormal = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	//float planeD = -66.0f;
	DirectX::XMVECTOR plane = DirectX::XMVectorSet(planeNormal.m128_f32[0], planeNormal.m128_f32[1], planeNormal.m128_f32[2], planeD);

	// Find intersection point of ray and plane
	DirectX::XMVECTOR intersectionPoint = DirectX::XMPlaneIntersectLine(
		plane,
		rayOrigin,
		DirectX::XMVectorAdd(rayOrigin, DirectX::XMVectorScale(rayDirToWorld, 1000.0f)) // Large distance in ray direction
	);

	return DirectX::XMFLOAT3(intersectionPoint.m128_f32[0], intersectionPoint.m128_f32[1], intersectionPoint.m128_f32[2]);
}

const DirectX::XMFLOAT4X4& ArkEngine::ArkDX11::DX11Renderer::GetViewMatrix()
{
	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMStoreFloat4x4(&viewMatrix, _mainCamera->GetViewMatrix());

	return viewMatrix;
}

const DirectX::XMFLOAT4X4& ArkEngine::ArkDX11::DX11Renderer::GetProjMatrix()
{
	DirectX::XMFLOAT4X4 projMatrix;
	DirectX::XMStoreFloat4x4(&projMatrix, _mainCamera->GetProjMatrix());

	return projMatrix;
}

const DirectX::XMFLOAT4X4& ArkEngine::ArkDX11::DX11Renderer::GetTransform(GInterface::GraphicsRenderable* renderable, const std::string& boneName)
{
	auto mat = renderable->GetBoneTransform(boneName);

	return mat;
}

const std::vector<std::vector<std::vector<DirectX::XMFLOAT3>>> ArkEngine::ArkDX11::DX11Renderer::GetAllMeshVertex()
{
	std::vector<std::vector<std::vector<DirectX::XMFLOAT3>>> vertexWorldList;

	for (auto index : ResourceManager::GetInstance()->GetRenderableList())
	{
		auto renderable = dynamic_cast<FBXMesh*>(index);
		renderable->GetWorldTransform();

		if (index->GetRenderingState())
		{
			auto buffer = ResourceManager::GetInstance()->GetArkBuffer(renderable->GetFileName());

			std::vector<std::vector<DirectX::XMFLOAT3>> parentVertexList;

			for (int i = 0; i < buffer.size(); i++)
			{
				std::vector<DirectX::XMFLOAT3> childVertexList;

				auto vertexList = buffer[i]->GetVertexPosList();

				for (auto vertex : vertexList)
				{
					auto newVertex = DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&vertex), renderable->GetWorldTransform());
					DirectX::XMFLOAT3 worldVertex;
					DirectX::XMStoreFloat3(&worldVertex, newVertex);
					childVertexList.emplace_back(worldVertex);
				}
				parentVertexList.emplace_back(childVertexList);
			}

			vertexWorldList.emplace_back(parentVertexList);
		}

	}

	return vertexWorldList;
}

const std::vector<std::vector<std::vector<unsigned int>>> ArkEngine::ArkDX11::DX11Renderer::GetAllMeshIndex()
{
	std::vector<std::vector<std::vector<unsigned int>>> totalIndexList;

	for (auto index : ResourceManager::GetInstance()->GetRenderableList())
	{
		auto renderable = dynamic_cast<FBXMesh*>(index);
		renderable->GetWorldTransform();

		if (index->GetRenderingState())
		{
			auto buffer = ResourceManager::GetInstance()->GetArkBuffer(renderable->GetFileName());

			std::vector<std::vector<unsigned int>> parentIndexList;

			for (int i = 0; i < buffer.size(); i++)
			{
				std::vector<unsigned int> childIndexList;

				childIndexList = buffer[i]->GetIndexList();

				parentIndexList.emplace_back(childIndexList);
			}

			totalIndexList.emplace_back(parentIndexList);
		}

	}

	return totalIndexList;
}


std::vector<DirectX::XMFLOAT3> ArkEngine::ArkDX11::DX11Renderer::GetMeshVertexData(const std::string& fileName)
{
	auto buffer = ResourceManager::GetInstance()->GetArkBuffer(fileName);

	return buffer[0]->GetVertexPosList();
}


std::vector<unsigned int> ArkEngine::ArkDX11::DX11Renderer::GetMeshIndexData(const std::string& fileName)
{
	auto buffer = ResourceManager::GetInstance()->GetArkBuffer(fileName);

	return buffer[0]->GetIndexList();
}


GInterface::GraphicsTransparentMesh* ArkEngine::ArkDX11::DX11Renderer::CreateTransParentMesh(const std::string& objectName, const std::string& textureName, float transParency, bool isCircle /*= false*/)
{
	auto newObject = new TransparentMesh(objectName, textureName, transParency, isCircle);

	GInterface::GraphicsTransparentMesh* tempObject = dynamic_cast<GInterface::GraphicsTransparentMesh*>(newObject);

	return tempObject;
}


void ArkEngine::ArkDX11::DX11Renderer::DeleteTransParentMesh(GInterface::GraphicsTransparentMesh* mesh)
{
	mesh->Delete();
}

void ArkEngine::ArkDX11::DX11Renderer::CreateDevice()
{
	// ����� ��� ���忡�� ����� ������ Ȱ��ȭ�ϱ� ���� �÷��� ����
#if defined(DEBUG) || defined(_DEBUG)
	UINT createDeviceFlags = 0;
	createDeviceFlags |= D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#else
	UINT createDeviceFlags = 0;
	createDeviceFlags |= D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#endif

	/// DEVICE�� DEVICECONTEXT ���ÿ� �����Ϸ��� D3D11CreateDeviceAndSwapChain�� ����ؾ� ��

	D3D_FEATURE_LEVEL featureLevel;

	HRESULT hr = D3D11CreateDevice(
		0,							// ����̽��� ������ �� ����� ���� ����Ϳ� ���� ������ -> �⺻ ����� ���� NULL�� ���� 
		D3D_DRIVER_TYPE_HARDWARE,	// �������� 3���� �׷��� ������ ����	// adapter�� NULL�� �ƴ� ������ ������ ��� D3D_DRIVER_TYPE_UNKNOWN���� �����Ͽ��� ��
		0,							// ����Ʈ ���� ��ġ ������� ����		// D3D_DRIVIER_TYPE_SOFRWARE�� ��� �̰��� NULL�̸� �ȵ�
		createDeviceFlags,		// �߰��� ��ġ ���� �÷���
		0,							// pfeatureLevels -> ���� ���ҵ��� �迭 -> ���ҵ��� ������ ��� ���ص��� �����ϴ� ���� -> �� ���� �⺻ �迭���� �ְ� ��� ���� ���õ� (11_0)
		0,							// pfeatureLevels�� ���� ����
		D3D11_SDK_VERSION,			// D11 SDK ����
		&_device,				// �� �Լ��� ������ ��ġ(device)�� �� ������ ������
		&featureLevel,			// �迭���� ó������ �����Ǵ� ��� ������ �� ������ ������
		&_deviceContext		// ������ ��ġ ���� (device context)�� �� ������ ������
	);

	if (FAILED(hr))
	{
		// MessageBox (hwnd, ��Ÿ�� ����, �޼����ڽ� �̸�, ���� ��ư ��)
		MessageBox(0, L"D3D11CreateDevice Failed", 0, 0);
	}

	if (featureLevel != D3D_FEATURE_LEVEL_11_0)
	{
		MessageBox(0, L"Direct3D Feature Level 11 unsupported", 0, 0);
	}
}

void ArkEngine::ArkDX11::DX11Renderer::CheckQualification()
{
	_device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &_4xMsaaQuality);
	// DXGI_FORMAT_R8G8B8A8_UNORM : �� ���Ҵ� [0,1] �������� ���Ǵ� 8��Ʈ ��ȣ���� ���� �� ���� �̷����
	assert(_4xMsaaQuality > 0);	// 4X MSAA�� �׻� �����ǹǷ� ��ȯ�� ǰ�� ������ 0���� ũ�� ������ ���� �Ѱ���
}

DXGI_SWAP_CHAIN_DESC ArkEngine::ArkDX11::DX11Renderer::GetSwapChainDesc(long long hwnd)
{
	DXGI_SWAP_CHAIN_DESC swapChainDesc;

	/// BufferDesc : �����ϰ��� �ϴ� �ĸ� ���۵��� �Ӽ�
// �ػ� ���� ��Ÿ���� ����, ���� ũ��
	swapChainDesc.BufferDesc.Width = _clientWidth;					// 0���� �����ϸ� ��Ÿ���� ���â���� �ʺ� ������ �� �ʺ��� ����ü�ο� �Ҵ���
	swapChainDesc.BufferDesc.Height = _clientHeight;					// 0���� �����ϸ� ��Ÿ���� ���â���� �ʺ� ������ �� �ʺ��� ����ü�ο� �Ҵ���
	// RefreshRate : ���ΰ�ħ �󵵸��� �츣�� ������ ����
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;				// ����
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;				// �и�, ������ ǥ���ϴ� ��� �и�� 1�̾�� ��
	// Format : ���÷��� ������ ����
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;		// �� ���Ҵ� [0,1] �������� ���Ǵ� 8��Ʈ ��ȣ���� ���� �� ���� �̷����
	// ScanlineOrdering : ��ĵ���� �׸��� ��带 �����ϴ� ������ ���
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	// Scaling : �����ϸ� ��带 �����ϴ� ������ ���
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// SampleDesc : ���� ǥ��ȭ�� ���� ������ ǥ�� ������ ǰ�� ���� -> ���� ������� �ʵ��� 1,0���� ����
	// ���߹��۸��� ����ϱ� ���ؼ��� ����ǥ��ȭ�� ����ؼ��ȵ�
	//if (_enable4xMsaa)
	//{
	//	// Count : �ȼ� �� ��Ƽ ���� ��
	//	swapChainDesc.SampleDesc.Count = 4;
	//	// Quality : �̹��� ǰ�� ���� -> CheckMultisampleQualityLevels�� ��ȯ�ϴ� �������� �۾ƾ� ��
	//	swapChainDesc.SampleDesc.Quality = _4xMsaaQuality - 1;
	//}
	//else

	{
		// ��Ƽ��������� ���� �⺻ ���÷� ��� 
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
	}

	// BufferUsage : ������ �뵵�� ����, �� ������ ǥ�� ��뷮�� cpu �׼��� �ɼ��� �����ϴ� ������ ��� -> �ĸ� ���ۿ� �������� �� ��
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// BufferCount : ��ȯ �罽���� ����� �ĸ� ������ ���� -> �Ϲ������� ���߹��۸��� �� �ϳ�, ���߹��۸��� �� �� ��
	swapChainDesc.BufferCount = 2;

	// OutputWindow : ������ ����� ǥ���� â�� �ڵ� -> â ���� ���� ü���� �����ϰ� ����ڰ� IDXGISwapChain::SetFullscreenState�� ���� ���� ü���� ��ü ȭ������ ������ �� �ֵ��� ����ϴ� ���� ����
	swapChainDesc.OutputWindow = (HWND)hwnd;

	// Windowed : â ��带 ���ϸ� true, ��üȭ�� ��带 ���ϸ� false
	swapChainDesc.Windowed = true;

	// SwapEffect : ǥ���� ǥ���� �� presentation ������ ������ ó���ϴ� �ɼ��� �����ϴ� ������ ����� DXGI_SWAP_EFFECT_DISCARD�� �����ϸ� ���÷��� �����Ⱑ ���� ȿ������ ���ù���� ����
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	// Flags : ���� ü�� ���ۿ� ���� �ɼ��� �����ϴ� ������ ��� -> DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH�� �����ϸ� ��üȭ�� ���� ��ȯ�� �� ������ �ĸ� ���� ������ ���� �� �����ϴ� ���÷��̰� �ڵ����� ���õ�
	swapChainDesc.Flags = 0;

	return swapChainDesc;
}

void ArkEngine::ArkDX11::DX11Renderer::CreateSwapChain(long long hwnd)
{
	// IDXGIDevice : �̹��� �����͸� �����ϴ� DXGI ��ü�� ���� �Ļ� Ŭ������ ����
	IDXGIDevice* p_dxgiDevice = 0;
	// queryinterface�� ù��° �Ű����� : COM ��ü���� ���� ��û�� IID�� �Ѱ���
	// queryinterface�� �ι�° �Ű����� : COM ��ü�� ������ �ִ� �������̽��� �� ������ �����
	_device->QueryInterface(__uuidof(IDXGIDevice), (void**)&p_dxgiDevice);

	// IDXGIAdapter : ���÷��� ����ý��� (�ϳ� �̻��� GPU, DAC �� ���� �޸� ����)�� ��Ÿ��
	IDXGIAdapter* p_dxgiAdapter = 0;
	p_dxgiDevice->GetAdapter(&p_dxgiAdapter);									// msdn������ �� ��� ���
	//p_dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&p_dxgiAdapter);	// å������ �� ��� ���

	// IDXGIFactory : ��ü ȭ�� ��ȯ�� ó���ϴ� DXGI ��ü�� �����ϴ� �ż��带 ����
	IDXGIFactory* p_dxgiFactory = 0;
	p_dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&p_dxgiFactory);

	// CreateSwapChain (ID3D11Device�� ����Ű�� ������, ��ȯ�罽 ���� ����ü�� ����Ű�� ������, ������ ��ȯ�罽 �������̽��� ������ ����)
	DXGI_SWAP_CHAIN_DESC tempSwapChainDesc = GetSwapChainDesc(hwnd);
	p_dxgiFactory->CreateSwapChain(_device.Get(), &tempSwapChainDesc, &_swapChain);

	// ȹ���� COM �������̽����� �� ��������Ƿ� ����
	p_dxgiFactory->Release();
	p_dxgiAdapter->Release();
	p_dxgiDevice->Release();
}

void ArkEngine::ArkDX11::DX11Renderer::CreateRenderTargetView()
{
	// ID3D11Texture2D : ����ȭ�� �޸��� �ؼ� �����͸� ����
	// �ؽ�ó�� ���������ο� ���� ���ε��� �� ������ �並 �����ϰ� ���ε� �ؾ� ��
	ID3D11Texture2D* backBuffer;

	// ù �Ű������� �ĸ� ������ ���� -> �ĸ� ���۰� �ϳ��̹Ƿ� 0�� ����
	// ��° �Ű������� ������ �������̽� ������ ���� -> �Ϲ������δ� 2���� �ؽ�Ʈ�� ���� ID3D11Texture2D�� ����
	// ��° �Ű������� �ĸ� ���۸� ����Ű�� �����͸� �� ������ ������
	_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**> (&backBuffer));

	// ù �Ű������� ���� ������� ����� �ڿ�
	// ��° �Ű������� d3d11_RENDER_TARGET_VIEW_DESC ����ü�� ����Ű�� ������, ������ �ڿ��� �ƴ� �� �� �Ű������� NULL���� �����ص� ��
	// ��° �Ű������� ���� ��� �並 ������
	if (backBuffer != nullptr)
	{
		_device->CreateRenderTargetView(backBuffer, 0, _renderTargetView.GetAddressOf());
	}
	// ȹ���� COM �������̽����� �� ��������Ƿ� ����
	backBuffer->Release();
}

void ArkEngine::ArkDX11::DX11Renderer::CreateDepthStencilView()
{
	D3D11_TEXTURE2D_DESC depthStencilDesc;

	ID3D11Texture2D* pDepthStencilBuffer;

	// Width : �ؽ�ó�� �ʺ� (1~16384)
	depthStencilDesc.Width = _clientWidth;

	// Height : �ؽ�ó�� ���� (1~16384)
	depthStencilDesc.Height = _clientHeight;

	// MipLevels : �Ӹ� ������ ���� -> ���� ���ٽ� ���۸� ���� �ؽ�ü������ �Ӹ� ������ �Ѱ��� ������ ��
	depthStencilDesc.MipLevels = 1;

	// ArraySize : �ؽ�ó �迭�� ���� -> ���� ���ٽ� ������ ��쿡�� �ؽ�ó �ϳ��� �ʿ� // 1~2048 ���̾�� ��
	depthStencilDesc.ArraySize = 1;

	// Format : �ؼ��� ������ ���ϴ� �ʵ�
	// �� �ؼ��� [0,1] �������� ���Ǵ� ��ȣ ���� 24��Ʈ ���� �� �ϳ��� [0.255] �������� ���Ǵ� 8��Ʈ ��ȣ���� ���� ���ٽ� ������ ����
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	// SampleDesc : ���� ǥ�� ������ ǰ�� ������ �����ϴ� ����ü -> 4X MSAA�� ȭ�� �ػ��� 4�� ũ���� �ĸ� ���� �ϳ��� ���� ���� �ϳ��� ���
	// -> ���� ���ٽ� ���۸� ���� ���� ǥ��ȭ ������ ���� ��� ���� ������ ��ġ�ؾ���

	// ���߹��۸��� ����ϱ����� 

	//if (_enable4xMsaa)
	//{
	//	depthStencilDesc.SampleDesc.Count = 4;
	//	depthStencilDesc.SampleDesc.Quality = _4xMsaaQuality - 1;
	//}
	//else

	{
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
	}

	// Usage : �ؽ�ó�� �뵵�� ���ϴ� �ʵ� 
	// D3D11_USAGE_DEFUALT -> �ڿ��� GPU�� �а� ��� �� �� ��� 
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;

	// BindFlags : �ڿ��� ���������ο� � ������ ���� �������� ���� -> ���� ���ٽ� ������ ��� D3D11_BIND_DEPTH_STENCIL �÷��׸� ����
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	// CPUAccessFlags : CPU�� �ڿ��� �����ϴ� ����� �����ϴ� �÷���, CPU �׼����� �ʿ����� ���� ��� 0�� ���
	depthStencilDesc.CPUAccessFlags = 0;

	// MiscFlags : ��Ÿ �÷��� ��� ���� ���ٽ� ���ۿ��� ������� ����
	depthStencilDesc.MiscFlags = 0;

	// ó�� �Ű������� ������ �ؽ�ó�� �����ϴ� ����ü��
	// ��° �Ű������� �ؽ�Ʈ�� ä�� �ʱ� �ڷḦ ����Ű�� �������ε� �� �ؽ�ó�� ���� ���Ľ� ���۷� ����� ���̱⿡ ���� �ڷḦ ä�� �ʿ䰡 ����
	// ����° �Ű������� ���� ���Ľ� ���۸� ����Ű�� �����͸� ������
	_device->CreateTexture2D(&depthStencilDesc, 0, &pDepthStencilBuffer);

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// CreateDepthStencilView : ���ҽ� �����Ϳ� ������ �ϱ����� ���� ���ٽ� �並 ����
	// ó�� �Ű������� �並 �����ϰ����ϴ� �ڿ��� �ǹ�
	// ��° �Ű������� D3D11_DEPTH_STENCIL_VIEW_DESC ����ü�� ����Ű�� ������
	// ������ ������ �����ؼ� �ڿ��� �����ߴٸ� �Ű������� �ΰ��� ����
	if (pDepthStencilBuffer != nullptr)
	{
		_device->CreateDepthStencilView(pDepthStencilBuffer, &depthStencilViewDesc, &_depthStencilView);
	}

	pDepthStencilBuffer->Release();
}

void ArkEngine::ArkDX11::DX11Renderer::BindView()
{
	// ù �Ű� ������ ������ �ϴ� ���� ����� ����
	// ��° �Ű� ������ ���������ο� ���� ���� ��� ����� ����Ű�� �����͵��� ���� �迭�� ù ���Ҹ� ����Ű�� ������
	// ��° �Ű� ������ ���������ο� ���� ���� ���Ľ� �並 ����Ű�� ������
	_deviceContext->OMSetRenderTargets(1, _renderTargetView.GetAddressOf(), _depthStencilView.Get());
}

void ArkEngine::ArkDX11::DX11Renderer::SetViewportWithDefaultCamera()
{
	_viewPort.TopLeftX = 0.0f;		// ����Ʈ ���簢���� x ��ǥ -> ����Ʈ�� ǥ���� ���� ���α׷� â�� Ŭ���̾�Ʈ ���� ���簢��
	_viewPort.TopLeftY = 0.0f;		// ����Ʈ ���簢���� y ��ǥ -> ����Ʈ�� ǥ���� ���� ���α׷� â�� Ŭ���̾�Ʈ ���� ���簢��
	_viewPort.Width = static_cast<float> (_clientWidth);		// ����Ʈ ���簢���� x ũ��
	_viewPort.Height = static_cast<float> (_clientHeight);	// ����Ʈ ���簢���� y ũ��
	_viewPort.MinDepth = 0.0f;		// �ּ� ���� ����	 -> Ư�� ȿ���� ���ϴ� ���� �ƴϸ� 0
	_viewPort.MaxDepth = 1.0f;		// �ִ� ���� ����	 -> Ư�� ȿ���� ���ϴ� ���� �ƴϸ� 1
	// ù �Ű������� ���� ����Ʈ�� ����, �ι�° �Ű������� ����Ʈ �迭�� ����Ű�� ������
	_deviceContext->RSSetViewports(1, &_viewPort);

	// ī�޶� ����
	DirectX::XMFLOAT3 pos = { 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 target = { 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 worldUp = { 0.0f, 0.0f, 0.0f };

	CreateDefaultCamera(pos, target, worldUp);
}

void ArkEngine::ArkDX11::DX11Renderer::SetViewport()
{
	_deviceContext->RSSetViewports(1, &_viewPort);
}

void ArkEngine::ArkDX11::DX11Renderer::CreateShadowViewPort(int shadowWidth, int shadowHeight)
{
	_shadowWidth = shadowWidth;
	_shadowHeight = shadowHeight;

	_shadowViewPort.TopLeftX = 0.0f;		// ����Ʈ ���簢���� x ��ǥ -> ����Ʈ�� ǥ���� ���� ���α׷� â�� Ŭ���̾�Ʈ ���� ���簢��
	_shadowViewPort.TopLeftY = 0.0f;		// ����Ʈ ���簢���� y ��ǥ -> ����Ʈ�� ǥ���� ���� ���α׷� â�� Ŭ���̾�Ʈ ���� ���簢��
	_shadowViewPort.Width = static_cast<float> (_shadowWidth);		// ����Ʈ ���簢���� x ũ��
	_shadowViewPort.Height = static_cast<float> (_shadowHeight);	// ����Ʈ ���簢���� y ũ��
	_shadowViewPort.MinDepth = 0.0f;		// �ּ� ���� ����	 -> Ư�� ȿ���� ���ϴ� ���� �ƴϸ� 0
	_shadowViewPort.MaxDepth = 1.0f;		// �ִ� ���� ����	 -> Ư�� ȿ���� ���ϴ� ���� �ƴϸ� 1
}

void ArkEngine::ArkDX11::DX11Renderer::CreateShadowCamera(const DirectX::XMFLOAT3& cameraPosition, const DirectX::XMFLOAT3& targetPosition)
{
	DirectX::XMFLOAT3 worldUp = { 0.0f, 1.0f, 0.0f };

	ArkEngine::ArkDX11::Camera* newCamera = new ArkEngine::ArkDX11::Camera(cameraPosition, targetPosition, worldUp);
	ICamera* iCamera = newCamera;

	iCamera->SetProjectionMatrix(DirectX::XMConvertToRadians(90.0f), GetShadowRatio(), 1.0f, 1000.0f, true);

	ResourceManager::GetInstance()->SetShadowCamera(iCamera);
}

void ArkEngine::ArkDX11::DX11Renderer::SetShadowViewport()
{
	_deviceContext->RSSetViewports(1, &_shadowViewPort);
}

void ArkEngine::ArkDX11::DX11Renderer::CreateRenderState()
{
	D3D11_RASTERIZER_DESC solidDesc;
	ZeroMemory(&solidDesc, sizeof(D3D11_RASTERIZER_DESC));
	solidDesc.FillMode = D3D11_FILL_SOLID;		// ����ü �������� ���
	solidDesc.CullMode = D3D11_CULL_BACK;		// �ĸ� �ﰢ���� ������ ���� (�⺻)
	// ī�޶� ���� �ð�������� ���� �ﰢ���� �������� ����, �ݽð�������� ���� �ﰢ���� �ĸ����� ���� (�⺻)
	solidDesc.FrontCounterClockwise = false;
	solidDesc.DepthClipEnable = true;
	_device->CreateRasterizerState(&solidDesc, _solidRS.GetAddressOf());

	D3D11_RASTERIZER_DESC wireDesc;
	ZeroMemory(&wireDesc, sizeof(D3D11_RASTERIZER_DESC));
	wireDesc.FillMode = D3D11_FILL_WIREFRAME;
	wireDesc.CullMode = D3D11_CULL_BACK;
	wireDesc.FrontCounterClockwise = false;
	wireDesc.DepthClipEnable = true;
	_device->CreateRasterizerState(&wireDesc, _wireRS.GetAddressOf());

	D3D11_RASTERIZER_DESC shadowDesc;
	ZeroMemory(&shadowDesc, sizeof(D3D11_RASTERIZER_DESC));
	shadowDesc.FillMode = D3D11_FILL_SOLID;
	shadowDesc.CullMode = D3D11_CULL_NONE;
	shadowDesc.FrontCounterClockwise = false;
	shadowDesc.DepthClipEnable = true;
	shadowDesc.DepthBias = 117;
	shadowDesc.SlopeScaledDepthBias = 16.0f;
	shadowDesc.DepthBiasClamp = 0.01;
	_device->CreateRasterizerState(&shadowDesc, _shadowRS.GetAddressOf());
}

void ArkEngine::ArkDX11::DX11Renderer::CreateDepthStecilState()
{
	D3D11_DEPTH_STENCIL_DESC equalDesc;
	ZeroMemory(&equalDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	equalDesc.DepthEnable = true;
	equalDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	equalDesc.DepthFunc = D3D11_COMPARISON_LESS;
	equalDesc.StencilEnable = true;
	equalDesc.StencilReadMask = 0xFF;
	equalDesc.StencilWriteMask = 0xFF;
	// Stencil operations if pixel is front-facing.
	equalDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	equalDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	equalDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	equalDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	// Stencil operations if pixel is back-facing.
	equalDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	equalDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	equalDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	equalDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	_device->CreateDepthStencilState(&equalDesc, _depthStencilState.GetAddressOf());

	D3D11_DEPTH_STENCIL_DESC equalDesc3;
	ZeroMemory(&equalDesc3, sizeof(D3D11_DEPTH_STENCIL_DESC));
	equalDesc3.DepthEnable = true;
	equalDesc3.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO; // ���� �� ����� ��Ȱ��ȭ
	equalDesc3.DepthFunc = D3D11_COMPARISON_LESS;
	equalDesc3.StencilEnable = true;
	equalDesc3.StencilReadMask = 0xFF;
	equalDesc3.StencilWriteMask = 0xFF;
	// Stencil operations if pixel is front-facing.
	equalDesc3.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	equalDesc3.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	equalDesc3.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	equalDesc3.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	// Stencil operations if pixel is back-facing.
	equalDesc3.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	equalDesc3.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	equalDesc3.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	equalDesc3.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	_device->CreateDepthStencilState(&equalDesc3, _depthStencilStateNoWrite.GetAddressOf());

	D3D11_DEPTH_STENCIL_DESC equalDesc2;
	ZeroMemory(&equalDesc2, sizeof(D3D11_DEPTH_STENCIL_DESC));
	equalDesc2.DepthEnable = false;
	equalDesc2.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	equalDesc2.DepthFunc = D3D11_COMPARISON_LESS;
	equalDesc2.StencilEnable = true;
	equalDesc2.StencilReadMask = 0xFF;
	equalDesc2.StencilWriteMask = 0xFF;
	// Stencil operations if pixel is front-facing.
	equalDesc2.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	equalDesc2.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	equalDesc2.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	equalDesc2.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	// Stencil operations if pixel is back-facing.
	equalDesc2.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	equalDesc2.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	equalDesc2.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	equalDesc2.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	_device->CreateDepthStencilState(&equalDesc2, _depthStencilStateDisable.GetAddressOf());
}


void ArkEngine::ArkDX11::DX11Renderer::CreateBlendState()
{
	// ���� ������Ʈ�� ������ �� ����� ����ü �ʱ�ȭ
	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(blendDesc));
	blendDesc.AlphaToCoverageEnable = FALSE;
	blendDesc.IndependentBlendEnable = FALSE; // ���� ���� Ÿ�ٸ� ����ϴ� ���

	// ���� �ɼ� ����
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	// ���� ������Ʈ ����
	_device->CreateBlendState(&blendDesc, &_blendState);
}

void ArkEngine::ArkDX11::DX11Renderer::SetResourceManager()
{
	ArkDevice* p_Device = new ArkDevice(_device.Get(), _deviceContext.Get(), _depthStencilView.Get(), _solidRS.Get(), _wireRS.Get(), _shadowRS.Get());
	ResourceManager::GetInstance()->SetResource("Device", p_Device);
}

void ArkEngine::ArkDX11::DX11Renderer::CreateFont()
{
	_font = std::make_unique<ArkEngine::ArkDX11::DWFont>(_swapChain.Get());
}

void ArkEngine::ArkDX11::DX11Renderer::CreateEffect()
{
	const auto& effectVec = ResourceManager::GetInstance()->FindSpecificResources("Resources/FX", "fx");

	for (const auto& index : effectVec)
	{
		if (index != "Resources/FX/LightHelper.fx")
		{
			ArkEffect* newEffect = new ArkEffect(index.c_str());
		}
	}
}

void ArkEngine::ArkDX11::DX11Renderer::CreateTexture()
{
	const auto& ddsVec = ResourceManager::GetInstance()->FindSpecificResources("Resources/Textures", "dds");

	for (const auto& index : ddsVec)
	{
		ResourceManager::GetInstance()->SetTextureNameList(index);
	}

	const auto& pngVec = ResourceManager::GetInstance()->FindSpecificResources("Resources/Textures", "png");

	for (const auto& index : pngVec)
	{
		ResourceManager::GetInstance()->SetTextureNameList(index);
	}

	const auto& jpgVec = ResourceManager::GetInstance()->FindSpecificResources("Resources/Textures", "jpg");

	for (const auto& index : jpgVec)
	{
		ResourceManager::GetInstance()->SetTextureNameList(index);
	}

	const auto& tgaVec = ResourceManager::GetInstance()->FindSpecificResources("Resources/Textures", "tga");

	for (const auto& index : tgaVec)
	{
		ResourceManager::GetInstance()->SetTextureNameList(index);
	}
}

void ArkEngine::ArkDX11::DX11Renderer::CreateNewTexture()
{
	const auto& existNameList = ResourceManager::GetInstance()->GetTextureNameList();

	const auto& ddsVec = ResourceManager::GetInstance()->FindSpecificResources("Resources/Textures", "dds");

	for (int i = 0; i < ddsVec.size(); i++)
	{
		for (int j = 0; j < existNameList.size(); j++)
		{
			if (ddsVec[i] == existNameList[j])
			{
				break;
			}
			else if (j == existNameList.size() - 1)
			{
				ArkTexture* newTexture = new ArkTexture(ddsVec[i].c_str());
			}
		}
	}


	const auto& pngVec = ResourceManager::GetInstance()->FindSpecificResources("Resources/Textures", "png");

	for (int i = 0; i < pngVec.size(); i++)
	{
		for (int j = 0; j < existNameList.size(); j++)
		{
			if (pngVec[i] == existNameList[j])
			{
				break;
			}
			else if (j == existNameList.size() - 1)
			{
				ArkTexture* newTexture = new ArkTexture(pngVec[i].c_str());
				ResourceManager::GetInstance()->SetTextureNameList(pngVec[i]);
			}
		}
	}

	const auto& jpgVec = ResourceManager::GetInstance()->FindSpecificResources("Resources/Textures", "jpg");

	for (int i = 0; i < jpgVec.size(); i++)
	{
		for (int j = 0; j < existNameList.size(); j++)
		{
			if (jpgVec[i] == existNameList[j])
			{
				break;
			}
			else if (j == existNameList.size() - 1)
			{
				ArkTexture* newTexture = new ArkTexture(jpgVec[i].c_str());
				ResourceManager::GetInstance()->SetTextureNameList(jpgVec[i]);
			}
		}
	}
}

void ArkEngine::ArkDX11::DX11Renderer::CreateASEParser()
{
	const auto& aseVec = ResourceManager::GetInstance()->FindSpecificResources("Resources/ASEFile", "ASE");

	for (const auto& index : aseVec)
	{
		ASEParser* newAseParser = new ASEParser();
		newAseParser->Load(index.c_str());
		ResourceManager::GetInstance()->AddASEParser(index, newAseParser);
	}
}

void ArkEngine::ArkDX11::DX11Renderer::CreateDefaultCamera(DirectX::XMFLOAT3& pos, DirectX::XMFLOAT3& target, DirectX::XMFLOAT3& worldUp)
{
	ICamera* newCamera = new ArkEngine::ArkDX11::Camera(pos, target, worldUp);

	ResourceManager::GetInstance()->AddCamera(newCamera);

	_mainCamera = newCamera;

	newCamera->SetMain(true);
}

void ArkEngine::ArkDX11::DX11Renderer::InitializeAssimpTool()
{
	_assimpTool = std::make_unique<ArkEngine::FBXLoader::AssimpTool>();
}

void ArkEngine::ArkDX11::DX11Renderer::InitializeWIC()
{
	CoInitialize(nullptr);

	// WIC Factory ����
	IWICImagingFactory* pWICFactory = nullptr;

	HRESULT hrCreateFactory = CoCreateInstance(
		CLSID_WICImagingFactory,
		nullptr,
		CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&pWICFactory)
	);

	if (FAILED(hrCreateFactory) || !pWICFactory)
	{
		// WIC Factory ���� ���� ó��
		CoUninitialize(); // �ʱ�ȭ ����
	}
}

float ArkEngine::ArkDX11::DX11Renderer::GetAspectRatio()
{
	return static_cast<float>((float)_clientWidth / (float)_clientHeight);
}

float ArkEngine::ArkDX11::DX11Renderer::GetShadowRatio()
{
	return static_cast<float>((float)_shadowWidth / (float)_shadowHeight);
}

void ArkEngine::ArkDX11::DX11Renderer::CreateDirLight(const DirectX::XMFLOAT4& ambient, const DirectX::XMFLOAT4& diffuse, const DirectX::XMFLOAT4& specular, const DirectX::XMFLOAT3& direction)
{
	ArkEngine::LightManager::GetInstance()->AddDirectionalLight(ambient, diffuse, specular, direction);
}

void ArkEngine::ArkDX11::DX11Renderer::CreatePoLight(const DirectX::XMFLOAT4& ambient, const DirectX::XMFLOAT4& diffuse, const DirectX::XMFLOAT4& specular, const DirectX::XMFLOAT3& position, float range, float attenuation)
{
	ArkEngine::LightManager::GetInstance()->AddPointLight(ambient, diffuse, specular, position, range, attenuation);
}
