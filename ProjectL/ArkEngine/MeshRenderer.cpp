#include "ResourceManager.h"
#include "IRenderable.h"
#include "ICamera.h"
#include "ArkDevice.h"
#include "ArkEffect.h"
#include "ArkBuffer.h"
#include "ArkTexture.h"
#include "d3dx11effect.h"
#include "CommonStruct.h"
#include "FBXMesh.h"
#include "FBXAnimator.h"
#include "ParsingStructs.h"
#include "ArkTexture.h"

#include "MeshRenderer.h"

ArkEngine::MeshRenderer::MeshRenderer(IRenderable* mesh)
	: _fileName(), _effectName(), _mainCamera(nullptr), _arkDevice(nullptr), _arkEffect(nullptr),
	_effect(nullptr), _tech(nullptr), _fxBoneTransforms(nullptr),
	_fxWorld(nullptr), _fxWorldInvTranspose(nullptr), _fxworldViewProj(nullptr),
	_fxTexTransform(nullptr), _fxMaterial(nullptr), _diffuseMap(nullptr), _normalMap(nullptr), _emissionMap(nullptr),
	_fxColor(nullptr), _fxCartoon(nullptr), _fxAlpha(nullptr), _isTransparentExist(false),
	_noiseMap(nullptr), _burnGradation(nullptr),
	_noiseMapSRV(nullptr), _burnGradationSRV(nullptr),
	_noiseMapName("Resources/Textures/Dissolve/DissolvePattern.png"),
	_burnGradationName("Resources/Textures/Dissolve/burngradient.png"),
	_isDissolve(false), _dissolveValue(1.0f), _isApplyingDecal(false), _fxApplayDecal(nullptr)
{
	Initialize(mesh);

	// 맵만 데칼에 영향받게 하기위해
	if (mesh->GetName().find("Map") != std::string::npos)
	{
		_isApplyingDecal = true;
	}
}

ArkEngine::MeshRenderer::~MeshRenderer()
{
	_fxApplayDecal->Release();
	_fxAlpha->Release();
	_fxCartoon->Release();
	_fxColor->Release();

	_emissionMap->Release();
	_normalMap->Release();
	_diffuseMap->Release();

	_fxMaterial->Release();
	_fxTexTransform->Release();
	_fxworldViewProj->Release();
	_fxWorldInvTranspose->Release();
	_fxWorld->Release();

	if (_effectName != "Resources/FX/BasicTexDeferred.fx")
	{
		_fxBoneTransforms->Release();
	}
}

void ArkEngine::MeshRenderer::AddMeshInList(IRenderable* mesh)
{
	_meshList.emplace_back(mesh);
	_shadowList.emplace_back(mesh);
}

void ArkEngine::MeshRenderer::DeleteMeshInList(IRenderable* mesh)
{
	_shadowList.erase(std::remove(_shadowList.begin(), _shadowList.end(), mesh), _shadowList.end());
	_meshList.erase(std::remove(_meshList.begin(), _meshList.end(), mesh), _meshList.end());
}

void ArkEngine::MeshRenderer::Render()
{
	_renderList.clear();

 	for (const auto& index : _meshList)
	{
		if (index->GetInsideFrustumState())
		{
			_renderList.emplace_back(index);
		}
	}

	// 컬링 적용된 메쉬는 그리지 않음
	if (_renderList.empty())
	{
		return;
	}

	auto deviceContext = _arkDevice->GetDeviceContext();

	deviceContext->IASetInputLayout(_arkEffect->GetInputLayOut());
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	deviceContext->RSSetState(_arkDevice->GetSolidRS());

	UINT stride = sizeof(ArkEngine::ArkDX11::Vertex);
	UINT offset = 0;


	/// Dissolve Effect
	timeMan -= 0.001f;
	if (timeMan <= 0.0f)
	{
		timeMan = 1.0f;
	}


	SetIsDissolve();
	SetDissolve();

	_noiseMap->SetResource(_noiseMapSRV);
	_burnGradation->SetResource(_burnGradationSRV);


	SetIsDissolveSRV(_isDissolve);
	SetDissolveSRV(_dissolveValue);

	DirectX::XMMATRIX texTransform = DirectX::XMMatrixIdentity();
	_fxTexTransform->SetMatrix(reinterpret_cast<float*>(&texTransform));

	_fxCartoon->SetBool(_renderList.back()->GetCartoonRenderingState());

	for (int i = 0; i < _vertexBuffer.size(); i++)
	{
		deviceContext->IASetVertexBuffers(0, 1, &_vertexBuffer[i], &stride, &offset);
		deviceContext->IASetIndexBuffer(_indexBuffer[i], DXGI_FORMAT_R32_UINT, 0);

		if (_material.size() > i)
		{
			_fxMaterial->SetRawValue(&_material[i], 0, sizeof(ArkEngine::ArkDX11::Material));
		}

		D3DX11_TECHNIQUE_DESC techDesc;
		_tech->GetDesc(&techDesc);

		auto nowMesh = _meshes[i];

		_alphaList.clear();
		_colorList.clear();
		_worldList.clear();
		_worldInvList.clear();
		_worldViewProjList.clear();

		_diffuseMap->SetResource(_meshList[0]->GetDiffuseSRV()[i]);
		_normalMap->SetResource(_meshList[0]->GetNormalSRV()[i]);
		_emissionMap->SetResource(_meshList[0]->GetEmmisionSRV()[i]);

		for (const auto& index : _renderList)
		{
			_colorList.emplace_back(index->GetColor());
			_alphaList.emplace_back(index->GetAlhpa());

			_isTransparentExist = index->GetIsDissolve();

			if (index->GetAlhpa() < 1.0f)
			{
				_isTransparentExist = true;
			}

			DirectX::XMMATRIX world = index->GetTransformMat();

			if (index->GetParentMesh() != nullptr)
			{
				auto parentMesh = index->GetParentMesh();
				auto parentOriginWorld = index->GetParentBoneTransform();
				auto parentBoneAnim = parentMesh->GetAnimator()->GetBoneAnimation(index->GetParentBoneIndex());

				auto transform = DirectX::XMMatrixMultiply(parentOriginWorld, DirectX::XMLoadFloat4x4(&parentBoneAnim));

				world = DirectX::XMMatrixMultiply(transform, world);

				DirectX::XMFLOAT4X4 finalWorld;
				DirectX::XMStoreFloat4x4(&finalWorld, world);
				_worldList.emplace_back(finalWorld);

				auto view = _mainCamera->GetViewMatrix();
				auto proj = _mainCamera->GetProjMatrix();

				auto worldViewProj = world * view * proj;

				DirectX::XMFLOAT4X4 finalWorldViewProj;
				DirectX::XMStoreFloat4x4(&finalWorldViewProj, worldViewProj);
				_worldViewProjList.emplace_back(finalWorldViewProj);
			}
			else
			{
				DirectX::XMFLOAT4X4 finalWorld;
				DirectX::XMStoreFloat4x4(&finalWorld, world);
				_worldList.emplace_back(finalWorld);

				auto view = _mainCamera->GetViewMatrix();
				auto proj = _mainCamera->GetProjMatrix();

				auto worldViewProj = world * view * proj;

				DirectX::XMFLOAT4X4 finalWorldViewProj;
				DirectX::XMStoreFloat4x4(&finalWorldViewProj, worldViewProj);
				_worldViewProjList.emplace_back(finalWorldViewProj);
			}

			DirectX::XMMATRIX worldCopy = world;
			worldCopy.r[3] = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
			DirectX::XMMATRIX worldInv = XMMatrixInverse(nullptr, worldCopy);
			DirectX::XMMATRIX worldInvTranspose = DirectX::XMMatrixTranspose(worldInv);
			DirectX::XMFLOAT4X4 finalInv;
			DirectX::XMStoreFloat4x4(&finalInv, worldInvTranspose);

			_worldInvList.emplace_back(finalInv);
		}

		_fxWorld->SetMatrixArray(reinterpret_cast<float*>(&_worldList[0]), 0, static_cast<uint32_t>(_worldList.size()));

		_fxWorldInvTranspose->SetMatrixArray(reinterpret_cast<float*>(&_worldInvList[0]), 0, static_cast<uint32_t>(_worldInvList.size()));

		_fxworldViewProj->SetMatrixArray(reinterpret_cast<float*>(&_worldViewProjList[0]), 0, static_cast<uint32_t>(_worldViewProjList.size()));

		/// 여기도 하드코딩, 애니메이션이 똑같이 적용됨
		auto animator = _renderList[0]->GetAnimator();
		if (animator != nullptr)
		{
			_fxBoneTransforms->SetMatrixArray(reinterpret_cast<float*>(&animator->_boneTransformMatrix[0]), 0, static_cast<uint32_t>(animator->_boneTransformMatrix.size()));
		}

		_fxColor->SetFloatVectorArray(reinterpret_cast<float*>(&_colorList[0]), 0, static_cast<uint32_t>(_colorList.size()));

		_fxAlpha->SetFloatArray(reinterpret_cast<float*>(&_alphaList[0]), 0, static_cast<uint32_t>(_alphaList.size()));

		_fxApplayDecal->SetBool(_isApplyingDecal);

		_tech->GetPassByIndex(0)->Apply(0, deviceContext);
		deviceContext->DrawIndexedInstanced(nowMesh->indexNum, _renderList.size(), 0, 0, 0);
	}
}


void ArkEngine::MeshRenderer::RenderForward()
{
	_renderList.clear();

	for (const auto& index : _meshList)
	{
		if (index->GetInsideFrustumState())
		{
			_renderList.emplace_back(index);
		}
	}

	// 컬링 적용된 메쉬는 그리지 않음
	if (_renderList.empty())
	{
		return;
	}

	auto deviceContext = _arkDevice->GetDeviceContext();

	deviceContext->IASetInputLayout(_arkEffect->GetInputLayOut());
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	deviceContext->RSSetState(_arkDevice->GetSolidRS());

	UINT stride = sizeof(ArkEngine::ArkDX11::Vertex);
	UINT offset = 0;


	/// Dissolve Effect
	timeMan -= 0.001f;
	if (timeMan <= 0.0f)
	{
		timeMan = 1.0f;
	}


	SetIsDissolve();
	SetDissolve();

	_noiseMap->SetResource(_noiseMapSRV);
	_burnGradation->SetResource(_burnGradationSRV);


	SetIsDissolveSRV(_isDissolve);
	SetDissolveSRV(_dissolveValue);

	DirectX::XMMATRIX texTransform = DirectX::XMMatrixIdentity();
	_fxTexTransform->SetMatrix(reinterpret_cast<float*>(&texTransform));

	_fxCartoon->SetBool(_renderList.back()->GetCartoonRenderingState());

	for (int i = 0; i < _vertexBuffer.size(); i++)
	{
		deviceContext->IASetVertexBuffers(0, 1, &_vertexBuffer[i], &stride, &offset);
		deviceContext->IASetIndexBuffer(_indexBuffer[i], DXGI_FORMAT_R32_UINT, 0);

		if (_material.size() > i)
		{
			_fxMaterial->SetRawValue(&_material[i], 0, sizeof(ArkEngine::ArkDX11::Material));
		}

		D3DX11_TECHNIQUE_DESC techDesc;
		_tech->GetDesc(&techDesc);

		auto nowMesh = _meshes[i];

		_alphaList.clear();
		_colorList.clear();
		_worldList.clear();
		_worldInvList.clear();
		_worldViewProjList.clear();

		_diffuseMap->SetResource(_meshList[0]->GetDiffuseSRV()[i]);
		_normalMap->SetResource(_meshList[0]->GetNormalSRV()[i]);
		_emissionMap->SetResource(_meshList[0]->GetEmmisionSRV()[i]);

		for (const auto& index : _renderList)
		{
			_colorList.emplace_back(index->GetColor());
			_alphaList.emplace_back(index->GetAlhpa());

			_isTransparentExist = index->GetIsDissolve();

			if (index->GetAlhpa() < 1.0f)
			{
				_isTransparentExist = true;
			}

			DirectX::XMMATRIX world = index->GetTransformMat();

			if (index->GetParentMesh() != nullptr)
			{
				auto parentMesh = index->GetParentMesh();
				auto parentOriginWorld = index->GetParentBoneTransform();
				auto parentBoneAnim = parentMesh->GetAnimator()->GetBoneAnimation(index->GetParentBoneIndex());

				auto transform = DirectX::XMMatrixMultiply(parentOriginWorld, DirectX::XMLoadFloat4x4(&parentBoneAnim));

				world = DirectX::XMMatrixMultiply(transform, world);

				DirectX::XMFLOAT4X4 finalWorld;
				DirectX::XMStoreFloat4x4(&finalWorld, world);
				_worldList.emplace_back(finalWorld);

				auto view = _mainCamera->GetViewMatrix();
				auto proj = _mainCamera->GetProjMatrix();

				auto worldViewProj = world * view * proj;

				DirectX::XMFLOAT4X4 finalWorldViewProj;
				DirectX::XMStoreFloat4x4(&finalWorldViewProj, worldViewProj);
				_worldViewProjList.emplace_back(finalWorldViewProj);
			}
			else
			{
				DirectX::XMFLOAT4X4 finalWorld;
				DirectX::XMStoreFloat4x4(&finalWorld, world);
				_worldList.emplace_back(finalWorld);

				auto view = _mainCamera->GetViewMatrix();
				auto proj = _mainCamera->GetProjMatrix();

				auto worldViewProj = world * view * proj;

				DirectX::XMFLOAT4X4 finalWorldViewProj;
				DirectX::XMStoreFloat4x4(&finalWorldViewProj, worldViewProj);
				_worldViewProjList.emplace_back(finalWorldViewProj);
			}

			DirectX::XMMATRIX worldCopy = world;
			worldCopy.r[3] = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
			DirectX::XMMATRIX worldInv = XMMatrixInverse(nullptr, worldCopy);
			DirectX::XMMATRIX worldInvTranspose = DirectX::XMMatrixTranspose(worldInv);
			DirectX::XMFLOAT4X4 finalInv;
			DirectX::XMStoreFloat4x4(&finalInv, worldInvTranspose);

			_worldInvList.emplace_back(finalInv);
		}

		_fxWorld->SetMatrixArray(reinterpret_cast<float*>(&_worldList[0]), 0, static_cast<uint32_t>(_worldList.size()));

		_fxWorldInvTranspose->SetMatrixArray(reinterpret_cast<float*>(&_worldInvList[0]), 0, static_cast<uint32_t>(_worldInvList.size()));

		_fxworldViewProj->SetMatrixArray(reinterpret_cast<float*>(&_worldViewProjList[0]), 0, static_cast<uint32_t>(_worldViewProjList.size()));

		/// 여기도 하드코딩, 애니메이션이 똑같이 적용됨
		auto animator = _renderList[0]->GetAnimator();
		if (animator != nullptr)
		{
			_fxBoneTransforms->SetMatrixArray(reinterpret_cast<float*>(&animator->_boneTransformMatrix[0]), 0, static_cast<uint32_t>(animator->_boneTransformMatrix.size()));
		}

		_fxColor->SetFloatVectorArray(reinterpret_cast<float*>(&_colorList[0]), 0, static_cast<uint32_t>(_colorList.size()));

		_fxAlpha->SetFloatArray(reinterpret_cast<float*>(&_alphaList[0]), 0, static_cast<uint32_t>(_alphaList.size()));

		_tech->GetPassByIndex(2)->Apply(0, deviceContext);
		deviceContext->DrawIndexedInstanced(nowMesh->indexNum, _renderList.size(), 0, 0, 0);
	}
}

void ArkEngine::MeshRenderer::ShadowRender()
{
	auto deviceContext = _arkDevice->GetDeviceContext();

	deviceContext->IASetInputLayout(_arkEffect->GetInputLayOut());
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	deviceContext->RSSetState(_arkDevice->GetShadowRS());

	UINT stride = sizeof(ArkEngine::ArkDX11::Vertex);
	UINT offset = 0;

	for (int i = 0; i < _transparentList.size(); i++)
	{
		if (_transparentList[i]->GetAlhpa() > 0.0f)
		{
			_meshList.emplace_back(_transparentList[i]);
			_transparentList.erase(_transparentList.begin() + i);
		}
	}

	for (int i = 0; i < _meshList.size(); i++)
	{
		if (_meshList[i]->GetAlhpa() <= 0.0f)
		{
			_transparentList.emplace_back(_meshList[i]);
			_meshList.erase(_meshList.begin() + i);
		}
	}

	// 컬링 적용된 메쉬는 그리지 않음
	if (_meshList.empty())
	{
		return;
	}

	for (int i = 0; i < _vertexBuffer.size(); i++)
	{
		deviceContext->IASetVertexBuffers(0, 1, &_vertexBuffer[i], &stride, &offset);
		deviceContext->IASetIndexBuffer(_indexBuffer[i], DXGI_FORMAT_R32_UINT, 0);

		D3DX11_TECHNIQUE_DESC techDesc;
		_tech->GetDesc(&techDesc);

		auto nowMesh = _meshes[i];

		_worldList.clear();
		_worldInvList.clear();
		_worldViewProjList.clear();

		for (const auto& index : _meshList)
		{
			DirectX::XMMATRIX world = index->GetTransformMat();

			if (index->GetParentMesh() != nullptr)
			{
				auto parentMesh = index->GetParentMesh();
				auto parentOriginWorld = index->GetParentBoneTransform();
				auto parentBoneAnim = parentMesh->GetAnimator()->GetBoneAnimation(index->GetParentBoneIndex());

				auto transform = DirectX::XMMatrixMultiply(parentOriginWorld, DirectX::XMLoadFloat4x4(&parentBoneAnim));

				world = DirectX::XMMatrixMultiply(transform, world);

				DirectX::XMFLOAT4X4 finalWorld;
				DirectX::XMStoreFloat4x4(&finalWorld, world);
				_worldList.emplace_back(finalWorld);

				auto view = _mainCamera->GetViewMatrix();
				auto proj = _mainCamera->GetProjMatrix();

				auto worldViewProj = world * view * proj;

				DirectX::XMFLOAT4X4 finalWorldViewProj;
				DirectX::XMStoreFloat4x4(&finalWorldViewProj, worldViewProj);
				_worldViewProjList.emplace_back(finalWorldViewProj);
			}
			else
			{
				DirectX::XMFLOAT4X4 finalWorld;
				DirectX::XMStoreFloat4x4(&finalWorld, world);
				_worldList.emplace_back(finalWorld);

				auto view = _mainCamera->GetViewMatrix();
				auto proj = _mainCamera->GetProjMatrix();

				auto worldViewProj = world * view * proj;

				DirectX::XMFLOAT4X4 finalWorldViewProj;
				DirectX::XMStoreFloat4x4(&finalWorldViewProj, worldViewProj);
				_worldViewProjList.emplace_back(finalWorldViewProj);
			}

			DirectX::XMMATRIX worldCopy = world;
			worldCopy.r[3] = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
			DirectX::XMMATRIX worldInv = XMMatrixInverse(nullptr, worldCopy);
			DirectX::XMMATRIX worldInvTranspose = DirectX::XMMatrixTranspose(worldInv);
			DirectX::XMFLOAT4X4 finalInv;
			DirectX::XMStoreFloat4x4(&finalInv, worldInvTranspose);

			_worldInvList.emplace_back(finalInv);
		}

		_fxWorld->SetMatrixArray(reinterpret_cast<float*>(&_worldList[0]), 0, static_cast<uint32_t>(_worldList.size()));

		_fxWorldInvTranspose->SetMatrixArray(reinterpret_cast<float*>(&_worldInvList[0]), 0, static_cast<uint32_t>(_worldInvList.size()));

		_fxworldViewProj->SetMatrixArray(reinterpret_cast<float*>(&_worldViewProjList[0]), 0, static_cast<uint32_t>(_worldViewProjList.size()));

		auto animator = _meshList[0]->GetAnimator();
		if (animator != nullptr)
		{
			_fxBoneTransforms->SetMatrixArray(reinterpret_cast<float*>(&animator->_boneTransformMatrix[0]), 0, static_cast<uint32_t>(animator->_boneTransformMatrix.size()));
		}

		_tech->GetPassByIndex(1)->Apply(0, deviceContext);
		deviceContext->DrawIndexedInstanced(nowMesh->indexNum, _meshList.size(), 0, 0, 0);
	}
}

void ArkEngine::MeshRenderer::SetMainCamera(ICamera* mainCamera)
{
	_mainCamera = mainCamera;
}


bool ArkEngine::MeshRenderer::GetAlphaExist()
{
	return _isTransparentExist;
}


const float ArkEngine::MeshRenderer::GetDepth()
{
	auto cameraPos = _mainCamera->GetCameraPos();

	if (_meshList.empty())
	{
		return 0;
	}

	return _meshList[0]->GetDepth(cameraPos);
}


const std::vector<ArkEngine::IRenderable*>& ArkEngine::MeshRenderer::GetMeshList()
{
	return _meshList;
}

const std::string& ArkEngine::MeshRenderer::GetName()
{
	return _fileName;
}

void ArkEngine::MeshRenderer::Initialize(IRenderable* mesh)
{
	_fileName = mesh->GetName();

	// 목록에 추가
	AddMeshInList(mesh);

	// 리소스 매니저에 추가
	ResourceManager::GetInstance()->AddMeshRenderer(this);

	for (auto index : ResourceManager::GetInstance()->GetArkBuffer(_fileName))
	{
		_vertexBuffer.emplace_back(index->GetVertexBuffer());
		_indexBuffer.emplace_back(index->GetIndexBuffer());
	}

	auto modelMaterialData = ResourceManager::GetInstance()->GetModelMaterial(std::wstring(_fileName.begin(), _fileName.end()));
	{
		_material = modelMaterialData->_material;
	}

	_meshes = ResourceManager::GetInstance()->GetFbxParsingData(_fileName);

	// Dissolve 이펙트를 위한 텍스쳐 설정
	SetDissolveTexture();


	SetEffect(mesh);
}



void ArkEngine::MeshRenderer::SetEffect(IRenderable* mesh)
{
	_effectName = mesh->GetEffectName();
	_arkEffect = ResourceManager::GetInstance()->GetResource<ArkEngine::ArkDX11::ArkEffect>(_effectName);
	_effect = _arkEffect->GetEffect();
	_arkDevice = ResourceManager::GetInstance()->GetResource<ArkEngine::ArkDX11::ArkDevice>("Device");

	_tech = _effect->GetTechniqueByIndex(0);

	if (_meshList.back()->GetAnimator() != nullptr)
	{
		_fxBoneTransforms = _effect->GetVariableByName("gBoneTransforms")->AsMatrix();
	}

	_fxWorld = _effect->GetVariableByName("gWorld")->AsMatrix();
	_fxWorldInvTranspose = _effect->GetVariableByName("gWorldInvTranspose")->AsMatrix();
	_fxworldViewProj = _effect->GetVariableByName("gWorldViewProj")->AsMatrix();
	_fxTexTransform = _effect->GetVariableByName("gTexTransform")->AsMatrix();
	_fxMaterial = _effect->GetVariableByName("gMaterial");

	_diffuseMap = _effect->GetVariableByName("gDiffuseMap")->AsShaderResource();
	_normalMap = _effect->GetVariableByName("gNormalMap")->AsShaderResource();
	_emissionMap = _effect->GetVariableByName("gEmissiveMap")->AsShaderResource();

	_fxColor = _effect->GetVariableByName("gColor")->AsVector();

	_fxCartoon = _effect->GetVariableByName("gCartoon")->AsScalar();

	_fxAlpha = _effect->GetVariableByName("gAlpha")->AsScalar();

	_fxApplayDecal = _effect->GetVariableByName("gApplyDecal")->AsScalar();

	_noiseMap = _effect->GetVariableByName("gNoiseTexture")->AsShaderResource();
	_burnGradation = _effect->GetVariableByName("gBurnTexture")->AsShaderResource();
	_dissolveValueEffect = _effect->GetVariableByName("gDissolveValue")->AsScalar();
	_isDisolveEffect = _effect->GetVariableByName("gIsDissolve")->AsScalar();
}


void ArkEngine::MeshRenderer::SetDissolve()
{
	_dissolveValue = _meshList[0]->GetDissolveValue();
}

float ArkEngine::MeshRenderer::GetDissolveValue()
{
	return _dissolveValue;
}

void ArkEngine::MeshRenderer::SetDissolveSRV(float value)
{
	_dissolveValueEffect->SetFloat(value);
}

void ArkEngine::MeshRenderer::SetIsDissolve()
{
	_isDissolve = _meshList[0]->GetIsDissolve();
}

void ArkEngine::MeshRenderer::SetIsDissolveSRV(bool isDissolve)
{
	_isDisolveEffect->SetBool(isDissolve);
}

void ArkEngine::MeshRenderer::SetDissolveTexture()
{
	auto noiseTexture = ResourceManager::GetInstance()->GetResource<ArkEngine::ArkDX11::ArkTexture>(_noiseMapName);
	ArkEngine::ArkDX11::ArkTexture* newNoiseMap = new ArkEngine::ArkDX11::ArkTexture(_noiseMapName.c_str());
	_noiseMapSRV = newNoiseMap->GetDiffuseMapSRV();

	auto burnTexture = ResourceManager::GetInstance()->GetResource<ArkEngine::ArkDX11::ArkTexture>(_burnGradationName);
	ArkEngine::ArkDX11::ArkTexture* newBurnTexture = new ArkEngine::ArkDX11::ArkTexture(_burnGradationName.c_str());
	_burnGradationSRV = newBurnTexture->GetDiffuseMapSRV();
}

