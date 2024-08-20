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

	// 후처리 전에는 일단 쉐도우맵 사이즈 여기서 지정
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

	// Frustum culling을 위해 그리지 않아도 업데이트 시키도록 변경
	for (const auto& index : ResourceManager::GetInstance()->GetDebugObjectList())
	{
		{
			index->Update(_mainCamera);
		}
	}

	for (const auto& index : ResourceManager::GetInstance()->GetRenderableList())
	{
		// 일단은 그려질 것만 업데이트
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


	// 디버그용
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

	// 광원의 위치에 카메라로 설정
	const auto& lightIndexList = LightManager::GetInstance()->GetActiveIndexList();

	// 빛이 있다면
	if (lightIndexList.empty() == false)
	{
		// Perspective Shadow를 구현하기 위해 카메라를 기존 시점에서 광원에서의 시점으로 바꿈
		_mainCamera = ResourceManager::GetInstance()->GetShadowCamera()[0];

		// 쉐도우 버퍼에 광원시점에서의 물체들의 깊이값을 기록할 준비 
		BeginShadowRender();

		for (const auto& index : ResourceManager::GetInstance()->GetAllMeshRenderer())
		{
			index->SetMainCamera(_mainCamera);
			index->ShadowRender();
		}

		// 광원 시점에서 기존 시점에서의 카메라로 되돌림
		_mainCamera = _originMainCamera;
	}

	// 디퍼드 버퍼에 기존 카메라 시점에서의 오브젝트들을 렌더링 하기 위한 준비
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

	//// UI, FONT 출력을 위해 기존 켜져있던 깊이 버퍼 끄기
	_deviceContext->OMSetDepthStencilState(_depthStencilStateDisable.Get(), 0);

	BeginFinalRender();

	BeginTransparentSet();

	// UI, FONT 출력을 위해 기존 켜져있던 깊이 버퍼 끄기
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


	// 최종적으로 디퍼드 버퍼를 합산한 결과물을 화면에 출력할 준비
	FinalRender();

	BeginTransparentSet();

	// 디퍼드 버퍼를 조합하여 빈 Texture2D에 완성된 화면을 출력함
	_deferredRenderer->Update(_mainCamera);
	_deferredRenderer->Render();

	_deviceContext->OMSetDepthStencilState(_depthStencilStateDisable.Get(), 0);

	EndTransparentSet();

	for (const auto& index : ResourceManager::GetInstance()->GetParticleList())
	{
		index->Draw(_mainCamera, _particleCamera, 1);
	}

	// 큐브맵 렌더링
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


	// 디버그 모드일 경우
	if (_isDebugMode)
	{
		// 프러스텀 컬링용, 메쉬 크기만큼의 디버그용 오브젝트 렌더링
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

		// 직선 오브젝트 렌더링
		for (const auto& index : ResourceManager::GetInstance()->GetLineObjectList())
		{
			index->Render();
		}

		// DrawDebugText를 통해 생성된 모든 디버그용 폰트 렌더링
		_deviceContext->OMSetDepthStencilState(_depthStencilStateDisable.Get(), 0);

		_font->RenderDebug();
	}

	// 디버그용이지만 일단 분리
	{
		// 프러스텀 컬링용, 메쉬 크기만큼의 디버그용 오브젝트 렌더링
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

	// UI IMAGE 렌더링
	for (const auto& index : ResourceManager::GetInstance()->GetUIImageList())
	{
		if (index->GetRenderingState())
		{
			// 0번 패스로 실질적인 UI IMAGE를 렌더링
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

	// 이걸 하면 warning이 줄어들긴하는데.. (ref, intref가 0인 것들이 사라짐)
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

	// 현재 하나의 Directional Light에 관해서만 그림자 처리가 가능하므로
	// 처음 만든 디렉셔널 라이트에 대해 카메라를 만듬
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
	// Present (모니터의 주사율, 동기화 방식 -> 0이 기본, 표시에 대한 플래그나 추가 옵션 -> 0이 기본)
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
	// 렌더링 컨텍스트에 블렌딩 스테이트 설정
	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	UINT sampleMask = 0xffffffff;

	// 블렌딩 해제
	_deviceContext->OMSetBlendState(nullptr, blendFactor, sampleMask);

	_deviceContext->OMSetDepthStencilState(_depthStencilStateDisable.Get(), 0);
}

void ArkEngine::ArkDX11::DX11Renderer::DrawDebugText(int posX, int posY, int fontSize, const char* text, ...)
{
	va_list vl;
	// text의 위치를 기반으로 가변인자를 생성
	va_start(vl, text);

	// 가변인자를 매개변수로 넘겨줄 수 없기에 문자열 배열에 담아서 저장
	char buffer[256];
	vsnprintf(buffer, sizeof(buffer), text, vl);

	std::string result(buffer);

	// 넘긴 문자열을 출력
	_font->RenderDebugText(posX, posY, result, fontSize);

	va_end(vl);

}

void ArkEngine::ArkDX11::DX11Renderer::DrawUIText(int posX, int posY, int fontSize, const DirectX::XMFLOAT4& color, const char* text, ...)
{
	va_list vl;
	// text의 위치를 기반으로 가변인자를 생성
	va_start(vl, text);

	// 가변인자를 매개변수로 넘겨줄 수 없기에 문자열 배열에 담아서 저장
	char buffer[256];
	vsnprintf(buffer, sizeof(buffer), text, vl);

	std::string result(buffer);

	// 넘긴 문자열을 출력
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
	// screen 좌표계를 ndc로 변환
	float normalizedX = ((float)mouseX / (float)_clientWidth) * 2 - 1;
	float normalizedY = 1 - ((float)mouseY / (float)_clientHeight) * 2;

	// ndc에 proj 역행렬을 곱해 view 공간으로 변환
	float viewX = normalizedX / _mainCamera->GetProjMatrix().r[0].m128_f32[0];
	float viewY = normalizedY / _mainCamera->GetProjMatrix().r[1].m128_f32[1];

	DirectX::XMVECTOR rayDir = DirectX::XMVectorSet(viewX, viewY, 1.0f, 1.0f);
	rayDir = DirectX::XMVector3Normalize(rayDir);

	DirectX::XMMATRIX invView = DirectX::XMMatrixInverse(nullptr, _mainCamera->GetViewMatrix());

	DirectX::XMVECTOR rayOrigin = DirectX::XMVectorSet(_mainCamera->GetCameraPos().x, _mainCamera->GetCameraPos().y, _mainCamera->GetCameraPos().z, 1.0f);

	// view 공간에 view 행렬의 역행렬을 곱해 world 공간으로 변환
	DirectX::XMVECTOR rayDirToWorld = DirectX::XMVector3TransformCoord(rayDir, invView);

	DirectX::XMVECTOR intersectionPoint = DirectX::XMPlaneIntersectLine(
		DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f),        // 평면의 방정식 계수
		rayOrigin,		// 라인의 시작점 (카메라 위치)
		rayDirToWorld   // 라인의 방향
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
	// 디버그 모드 빌드에서 디버그 계층을 활성화하기 위해 플래그 설정
#if defined(DEBUG) || defined(_DEBUG)
	UINT createDeviceFlags = 0;
	createDeviceFlags |= D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#else
	UINT createDeviceFlags = 0;
	createDeviceFlags |= D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#endif

	/// DEVICE와 DEVICECONTEXT 동시에 생성하려면 D3D11CreateDeviceAndSwapChain을 사용해야 함

	D3D_FEATURE_LEVEL featureLevel;

	HRESULT hr = D3D11CreateDevice(
		0,							// 디바이스를 생성할 때 사용할 비디오 어댑터에 대한 포인터 -> 기본 어댑터 사용시 NULL을 전달 
		D3D_DRIVER_TYPE_HARDWARE,	// 랜더링에 3차원 그래픽 가속이 적용	// adapter를 NULL이 아닌 값으로 설정할 경우 D3D_DRIVER_TYPE_UNKNOWN으로 설정하여야 함
		0,							// 소프트 웨어 장치 사용하지 않음		// D3D_DRIVIER_TYPE_SOFRWARE일 경우 이것이 NULL이면 안됨
		createDeviceFlags,		// 추가적 장치 생성 플래그
		0,							// pfeatureLevels -> 형식 원소들의 배열 -> 원소들의 순서가 기능 수준들을 점검하는 순서 -> 널 값시 기본 배열에서 최고 기능 수준 선택됨 (11_0)
		0,							// pfeatureLevels의 원소 개수
		D3D11_SDK_VERSION,			// D11 SDK 버젼
		&_device,				// 이 함수가 생성한 장치(device)를 이 변수에 돌려줌
		&featureLevel,			// 배열에서 처음으로 지원되는 기능 수준을 이 변수에 돌려줌
		&_deviceContext		// 생성된 장치 문맥 (device context)을 이 변수에 돌려줌
	);

	if (FAILED(hr))
	{
		// MessageBox (hwnd, 나타날 문자, 메세지박스 이름, 누를 버튼 값)
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
	// DXGI_FORMAT_R8G8B8A8_UNORM : 각 원소는 [0,1] 구간으로 사상되는 8비트 부호없는 성분 네 개로 이루어짐
	assert(_4xMsaaQuality > 0);	// 4X MSAA가 항상 지원되므로 반환된 품질 수준이 0보다 크지 않으면 오류 넘겨줌
}

DXGI_SWAP_CHAIN_DESC ArkEngine::ArkDX11::DX11Renderer::GetSwapChainDesc(long long hwnd)
{
	DXGI_SWAP_CHAIN_DESC swapChainDesc;

	/// BufferDesc : 생성하고자 하는 후면 버퍼들의 속성
// 해상도 폭을 나타내는 가로, 세로 크기
	swapChainDesc.BufferDesc.Width = _clientWidth;					// 0으로 지정하면 런타임이 출력창에서 너비를 가져와 그 너비값을 스왑체인에 할당함
	swapChainDesc.BufferDesc.Height = _clientHeight;					// 0으로 지정하면 런타임이 출력창에서 너비를 가져와 그 너비값을 스왑체인에 할당함
	// RefreshRate : 새로고침 빈도르르 헤르츠 단위로 설정
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;				// 분자
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;				// 분모, 정수를 표현하는 경우 분모는 1이어야 함
	// Format : 디스플레이 포맷을 설정
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;		// 각 원소는 [0,1] 구간으로 사상되는 8비트 부호없는 성분 네 개로 이루어짐
	// ScanlineOrdering : 스캔라인 그리기 모드를 설명하는 열거형 멤버
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	// Scaling : 스케일링 모드를 설명하는 열거형 멤버
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// SampleDesc : 다중 표본화를 위해 추출할 표본 개수와 품질 수준 -> 현재 사용하지 않도록 1,0으로 지정
	// 삼중버퍼링을 사용하기 위해서는 다중표본화를 사용해선안됨
	//if (_enable4xMsaa)
	//{
	//	// Count : 픽셀 당 멀티 셈플 수
	//	swapChainDesc.SampleDesc.Count = 4;
	//	// Quality : 이미지 품질 수준 -> CheckMultisampleQualityLevels가 반환하는 레벨보다 작아야 함
	//	swapChainDesc.SampleDesc.Quality = _4xMsaaQuality - 1;
	//}
	//else

	{
		// 안티엘리어싱이 없는 기본 셈플러 모드 
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
	}

	// BufferUsage : 버퍼의 용도를 서술, 백 버퍼의 표면 사용량과 cpu 액세스 옵션을 설명하는 열거형 멤버 -> 후면 버퍼에 렌더링을 할 것
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// BufferCount : 교환 사슬에서 사용할 후면 버퍼의 갯수 -> 일반적으로 이중버퍼링일 때 하나, 삼중버퍼링일 때 두 개
	swapChainDesc.BufferCount = 2;

	// OutputWindow : 렌더링 결과를 표시할 창의 핸들 -> 창 모드로 스왑 체인을 생성하고 사용자가 IDXGISwapChain::SetFullscreenState를 통해 스왑 체인을 전체 화면으로 변경할 수 있도록 허용하는 것이 좋음
	swapChainDesc.OutputWindow = (HWND)hwnd;

	// Windowed : 창 모드를 원하면 true, 전체화면 모드를 원하면 false
	swapChainDesc.Windowed = true;

	// SwapEffect : 표면을 표시한 후 presentation 버퍼의 내용을 처리하는 옵션을 설명하는 열거형 멤버로 DXGI_SWAP_EFFECT_DISCARD를 지정하면 디스플레이 구동기가 가장 효율적인 제시방법을 선택
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	// Flags : 스왑 체인 동작에 대한 옵션을 설명하는 열거형 멤버 -> DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH를 지정하면 전체화면 모드로 전환될 때 현재의 후면 버퍼 설정에 가장 잘 부합하는 디스플레이가 자동으로 선택됨
	swapChainDesc.Flags = 0;

	return swapChainDesc;
}

void ArkEngine::ArkDX11::DX11Renderer::CreateSwapChain(long long hwnd)
{
	// IDXGIDevice : 이미지 데이터를 생성하는 DXGI 객체에 대한 파생 클래스를 구현
	IDXGIDevice* p_dxgiDevice = 0;
	// queryinterface의 첫번째 매개변수 : COM 객체에서 내가 요청한 IID를 넘겨줌
	// queryinterface의 두번째 매개변수 : COM 객체가 가지고 있는 인터페이스를 이 변수에 담아줌
	_device->QueryInterface(__uuidof(IDXGIDevice), (void**)&p_dxgiDevice);

	// IDXGIAdapter : 디스플레이 서브시스템 (하나 이상의 GPU, DAC 및 비디오 메모리 포함)을 나타냄
	IDXGIAdapter* p_dxgiAdapter = 0;
	p_dxgiDevice->GetAdapter(&p_dxgiAdapter);									// msdn에서는 이 방법 사용
	//p_dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&p_dxgiAdapter);	// 책에서는 이 방법 사용

	// IDXGIFactory : 전체 화면 전환을 처리하는 DXGI 객체를 생성하는 매서드를 구현
	IDXGIFactory* p_dxgiFactory = 0;
	p_dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&p_dxgiFactory);

	// CreateSwapChain (ID3D11Device를 가리키는 포인터, 교환사슬 서술 구조체를 가리키는 포인터, 생성된 교환사슬 인터페이스를 돌려줄 변수)
	DXGI_SWAP_CHAIN_DESC tempSwapChainDesc = GetSwapChainDesc(hwnd);
	p_dxgiFactory->CreateSwapChain(_device.Get(), &tempSwapChainDesc, &_swapChain);

	// 획득한 COM 인터페이스들을 다 사용했으므로 해제
	p_dxgiFactory->Release();
	p_dxgiAdapter->Release();
	p_dxgiDevice->Release();
}

void ArkEngine::ArkDX11::DX11Renderer::CreateRenderTargetView()
{
	// ID3D11Texture2D : 구조화된 메모리인 텍셀 데이터를 관리
	// 텍스처는 파이프라인에 직접 바인딩할 수 없으며 뷰를 생성하고 바인딩 해야 함
	ID3D11Texture2D* backBuffer;

	// 첫 매개변수는 후면 버퍼의 색인 -> 후면 버퍼가 하나이므로 0을 지정
	// 둘째 매개변수는 버퍼의 인터페이스 형식을 지정 -> 일반적으로는 2차원 텍스트를 위한 ID3D11Texture2D가 사용됨
	// 셋째 매개변수는 후면 버퍼를 가리키는 포인터를 이 변수에 돌려줌
	_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**> (&backBuffer));

	// 첫 매개변수는 렌더 대상으로 사용할 자원
	// 둘째 매개변수는 d3d11_RENDER_TARGET_VIEW_DESC 구조체를 가리키는 포인터, 무형식 자원이 아닐 시 이 매개변수에 NULL값을 지정해도 됨
	// 셋째 매개변수는 렌더 대상 뷰를 돌려줌
	if (backBuffer != nullptr)
	{
		_device->CreateRenderTargetView(backBuffer, 0, _renderTargetView.GetAddressOf());
	}
	// 획득한 COM 인터페이스들을 다 사용했으므로 해제
	backBuffer->Release();
}

void ArkEngine::ArkDX11::DX11Renderer::CreateDepthStencilView()
{
	D3D11_TEXTURE2D_DESC depthStencilDesc;

	ID3D11Texture2D* pDepthStencilBuffer;

	// Width : 텍스처의 너비 (1~16384)
	depthStencilDesc.Width = _clientWidth;

	// Height : 텍스처의 높이 (1~16384)
	depthStencilDesc.Height = _clientHeight;

	// MipLevels : 밉맵 수준의 개수 -> 깊이 스텐실 버퍼를 위한 텍스체에서는 밉맵 수준이 한개만 있으면 됨
	depthStencilDesc.MipLevels = 1;

	// ArraySize : 텍스처 배열의 개수 -> 깊이 스텐실 버퍼의 경우에는 텍스처 하나만 필요 // 1~2048 사이어야 함
	depthStencilDesc.ArraySize = 1;

	// Format : 텍셀의 형식을 뜻하는 필드
	// 각 텍셀은 [0,1] 구간으로 사상되는 부호 없는 24비트 깊이 값 하나와 [0.255] 구간으로 사상되는 8비트 부호없는 정수 스텐실 값으로 구성
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	// SampleDesc : 다중 표본 개수와 품질 수준을 서술하는 구조체 -> 4X MSAA는 화면 해상도의 4배 크기인 후면 버퍼 하나와 깊이 버퍼 하나를 사용
	// -> 깊이 스텐실 버퍼를 위한 다중 표본화 설정은 렌더 대상에 쓰인 설정과 일치해야함

	// 삼중버퍼링을 사용하기위해 

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

	// Usage : 텍스처의 용도를 뜻하는 필드 
	// D3D11_USAGE_DEFUALT -> 자원을 GPU가 읽고 써야 할 때 사용 
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;

	// BindFlags : 자원을 파이프라인에 어떤 식으로 묶을 것인지를 지정 -> 깊이 스텐실 버퍼의 경우 D3D11_BIND_DEPTH_STENCIL 플래그를 지정
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	// CPUAccessFlags : CPU가 자원에 접근하는 방식을 결정하는 플래그, CPU 액세스가 필요하지 않은 경우 0을 사용
	depthStencilDesc.CPUAccessFlags = 0;

	// MiscFlags : 기타 플래그 들로 깊이 스텐실 버퍼에는 적용되지 않음
	depthStencilDesc.MiscFlags = 0;

	// 처음 매개변수는 생성할 텍스처를 서술하는 구조체임
	// 둘째 매개변수는 텍스트에 채울 초기 자료를 가리키는 포인터인데 이 텍스처는 깊이 스탠실 버퍼로 사용할 것이기에 따로 자료를 채울 필요가 없음
	// 세번째 매개변수에 깊이 스탠실 버퍼를 가리키는 포인터를 돌려줌
	_device->CreateTexture2D(&depthStencilDesc, 0, &pDepthStencilBuffer);

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// CreateDepthStencilView : 리소스 데이터에 엑세스 하기위해 깊이 스텐실 뷰를 만듬
	// 처음 매개변수는 뷰를 생성하고자하는 자원을 의미
	// 둘째 매개변수는 D3D11_DEPTH_STENCIL_VIEW_DESC 구조체를 가리키는 포인터
	// 형식을 완전히 지정해서 자원을 생성했다면 매개변수에 널값을 지정
	if (pDepthStencilBuffer != nullptr)
	{
		_device->CreateDepthStencilView(pDepthStencilBuffer, &depthStencilViewDesc, &_depthStencilView);
	}

	pDepthStencilBuffer->Release();
}

void ArkEngine::ArkDX11::DX11Renderer::BindView()
{
	// 첫 매개 변수는 묶고자 하는 랜더 대상의 개수
	// 둘째 매개 변수는 파이프라인에 묶을 랜더 대상 뷰들을 가리키는 포인터들을 담은 배열의 첫 원소를 가리키는 포인터
	// 셋째 매개 변수는 파이프라인에 묶을 깊이 스탠실 뷰를 가리키는 포인터
	_deviceContext->OMSetRenderTargets(1, _renderTargetView.GetAddressOf(), _depthStencilView.Get());
}

void ArkEngine::ArkDX11::DX11Renderer::SetViewportWithDefaultCamera()
{
	_viewPort.TopLeftX = 0.0f;		// 뷰포트 직사각형의 x 좌표 -> 뷰포트를 표시할 응용 프로그램 창의 클라이언트 영역 직사각형
	_viewPort.TopLeftY = 0.0f;		// 뷰포트 직사각형의 y 좌표 -> 뷰포트를 표시할 응용 프로그램 창의 클라이언트 영역 직사각형
	_viewPort.Width = static_cast<float> (_clientWidth);		// 뷰포트 직사각형의 x 크기
	_viewPort.Height = static_cast<float> (_clientHeight);	// 뷰포트 직사각형의 y 크기
	_viewPort.MinDepth = 0.0f;		// 최소 깊이 버퍼	 -> 특수 효과를 원하는 것이 아니면 0
	_viewPort.MaxDepth = 1.0f;		// 최대 깊이 버퍼	 -> 특수 효과를 원하는 것이 아니면 1
	// 첫 매개변수는 묶을 뷰포트의 갯수, 두번째 매개변수는 뷰포트 배열을 가르키는 포인터
	_deviceContext->RSSetViewports(1, &_viewPort);

	// 카메라 생성
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

	_shadowViewPort.TopLeftX = 0.0f;		// 뷰포트 직사각형의 x 좌표 -> 뷰포트를 표시할 응용 프로그램 창의 클라이언트 영역 직사각형
	_shadowViewPort.TopLeftY = 0.0f;		// 뷰포트 직사각형의 y 좌표 -> 뷰포트를 표시할 응용 프로그램 창의 클라이언트 영역 직사각형
	_shadowViewPort.Width = static_cast<float> (_shadowWidth);		// 뷰포트 직사각형의 x 크기
	_shadowViewPort.Height = static_cast<float> (_shadowHeight);	// 뷰포트 직사각형의 y 크기
	_shadowViewPort.MinDepth = 0.0f;		// 최소 깊이 버퍼	 -> 특수 효과를 원하는 것이 아니면 0
	_shadowViewPort.MaxDepth = 1.0f;		// 최대 깊이 버퍼	 -> 특수 효과를 원하는 것이 아니면 1
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
	solidDesc.FillMode = D3D11_FILL_SOLID;		// 고형체 렌더링인 경우
	solidDesc.CullMode = D3D11_CULL_BACK;		// 후면 삼각형을 선별해 제외 (기본)
	// 카메라 기준 시계방향으로 감긴 삼각형을 전면으로 간주, 반시계방향으로 감긴 삼각형을 후면으로 간주 (기본)
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
	equalDesc3.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO; // 깊이 값 기록을 비활성화
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
	// 블렌딩 스테이트를 생성할 때 사용할 구조체 초기화
	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(blendDesc));
	blendDesc.AlphaToCoverageEnable = FALSE;
	blendDesc.IndependentBlendEnable = FALSE; // 단일 렌더 타겟만 사용하는 경우

	// 블렌딩 옵션 설정
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	// 블렌딩 스테이트 생성
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

	// WIC Factory 생성
	IWICImagingFactory* pWICFactory = nullptr;

	HRESULT hrCreateFactory = CoCreateInstance(
		CLSID_WICImagingFactory,
		nullptr,
		CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&pWICFactory)
	);

	if (FAILED(hrCreateFactory) || !pWICFactory)
	{
		// WIC Factory 생성 실패 처리
		CoUninitialize(); // 초기화 해제
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
