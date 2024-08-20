#define NOMINMAX

#include <filesystem>
#include <DirectXMath.h>
#include <algorithm>
#include "ResourceManager.h"
#include "ArkEffect.h"
#include "d3dx11effect.h"
#include "Camera.h"
#include "ArkDevice.h"
#include "ArkTexture.h"
#include "CommonStruct.h"
#include "ASEParser.h"
#include "ArkBuffer.h"
#include "Transform.h"
#include "Animator.h"
#include "DirectionalLight.h"
#include "LightManager.h"
#include "GeometryGenerator.h"
#include "DebugObject.h"
#include "ASEMesh.h"

ArkEngine::ArkDX11::ASEMesh::ASEMesh(const std::string& fileName, const std::string& textureName)
	: _fileName(fileName), _effectName("Resources/FX/BasicTexDeferred.fx"), _textureName(textureName), _transform(),
	_effect(nullptr), _tech(nullptr),
	_fxWorld(nullptr), _fxWorldInvTranspose(nullptr), _fxWorldViewProj(nullptr), _fxTexTransform(nullptr), _fxMaterial(nullptr),
	_fxBoneTransforms(nullptr),
	_diffuseMap(nullptr), _diffuseMapSRV(nullptr), _normalMap(nullptr), _normalMapSRV(nullptr),
	_cubeMap(nullptr), _boneTMList(), _world(), _view(), _proj(), _vertexBuffer(nullptr), _indexBuffer(nullptr),
	_material(),
	_arkDevice(nullptr), _arkEffect(nullptr), _totalVertexCount(0), _totalIndexCount(0), _meshTransform(nullptr),
	_aseParser(nullptr), _animator(nullptr),
	_isStaticMesh(true), _isRendering(true), _myIndex(0),
	_debugObject(nullptr)
{
	Initialize();
}

ArkEngine::ArkDX11::ASEMesh::~ASEMesh()
{
	Finalize();
}

void ArkEngine::ArkDX11::ASEMesh::Initialize()
{
	BuildGeometryBuffers();

	if (_isStaticMesh == false)
	{
		_effectName = "Resources/FX/SkinningDeferred.fx";
	}

	_arkEffect = ResourceManager::GetInstance()->GetResource<ArkEngine::ArkDX11::ArkEffect>(_effectName);
	_effect = _arkEffect->GetEffect();

	_arkDevice = ResourceManager::GetInstance()->GetResource<ArkEngine::ArkDX11::ArkDevice>("Device");

	_meshTransform = new Transform();

	SetEffect();

	SetBasicMaterial();

	SetLight();

	SetAnimator();

	_debugObject = new DebugObject(_fileName, DebugObject::eDebugType::Cube);
}

void ArkEngine::ArkDX11::ASEMesh::Update(ArkEngine::ICamera* p_Camera)
{
	auto camera = static_cast<ArkEngine::ArkDX11::Camera*>(p_Camera);

	DirectX::XMStoreFloat4x4(&_world, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&_view, camera->GetViewMatrix());
	DirectX::XMStoreFloat4x4(&_proj, camera->GetProjMatrix());
}

void ArkEngine::ArkDX11::ASEMesh::Render()
{
	auto deviceContext = _arkDevice->GetDeviceContext();

	deviceContext->IASetInputLayout(_arkEffect->GetInputLayOut());
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	deviceContext->RSSetState(_arkDevice->GetSolidRS());

	UINT stride = sizeof(ArkEngine::ArkDX11::Vertex);
	UINT offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &_vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	DirectX::XMMATRIX texTransform = DirectX::XMMatrixIdentity();
	_fxTexTransform->SetMatrix(reinterpret_cast<float*>(&texTransform));

	_fxMaterial->SetRawValue(&_material, 0, sizeof(Material));

	if (_diffuseMapSRV != nullptr)
	{
		_diffuseMap->SetResource(_diffuseMapSRV);
	}

	if (_normalMapSRV != nullptr)
	{
		_normalMap->SetResource(_normalMapSRV);
	}

	auto nowCubeMap = ResourceManager::GetInstance()->GetNowCubeMap();

	if (nowCubeMap != nullptr)
	{
		auto cubeTexture = ResourceManager::GetInstance()->GetResource<ArkTexture>(nowCubeMap->GetTextureName());
		_cubeMap->SetResource(cubeTexture->GetDiffuseMapSRV());
	}

	D3DX11_TECHNIQUE_DESC techDesc;
	_tech->GetDesc(&techDesc);

	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		unsigned sumIndexCount = 0;
		unsigned sumVertexCount = 0;

		for (auto nowMesh : _aseParser->GetMeshList())
		{
			_world = nowMesh->_newWorldTM.at(_myIndex);
			DirectX::XMMATRIX world = XMLoadFloat4x4(&_world);
			world = XMMatrixMultiply(world, _meshTransform->GetTransformMatrix());

			DirectX::XMMATRIX view = XMLoadFloat4x4(&_view);
			DirectX::XMMATRIX proj = XMLoadFloat4x4(&_proj);
			DirectX::XMMATRIX WorldViewProj = world * view * proj;

			DirectX::XMMATRIX worldCopy = world;
			worldCopy.r[3] = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
			DirectX::XMMATRIX worldInv = XMMatrixInverse(nullptr, worldCopy);
			DirectX::XMMATRIX worldInvTranspose = DirectX::XMMatrixTranspose(worldInv);

			_fxWorld->SetMatrix(reinterpret_cast<float*>(&world));
			_fxWorldInvTranspose->SetMatrix(reinterpret_cast<float*>(&worldInvTranspose));
			_fxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&WorldViewProj));

			if (_isStaticMesh == false)
			{
				if (nowMesh->m_isBone == false)
				{
					for (int j = 0; j < nowMesh->m_BoneList.size(); j++)
					{
						DirectX::XMMATRIX boneOriginWorld = DirectX::XMLoadFloat4x4(&nowMesh->m_BoneList[j]->_originWorldTM);
						DirectX::XMMATRIX boneNewWorld = DirectX::XMLoadFloat4x4(&nowMesh->m_BoneList[j]->_newWorldTM.at(_myIndex));

						DirectX::XMMATRIX skinWorld = DirectX::XMLoadFloat4x4(&nowMesh->_newWorldTM.at(_myIndex));
						DirectX::XMMATRIX skinInverse = DirectX::XMMatrixInverse(nullptr, skinWorld);

						// Origin bne의 월드에서 skinMesh의 월드를 기준으로 변환한 bone의 로컬행렬
						DirectX::XMMATRIX boneOffset = DirectX::XMMatrixMultiply(boneOriginWorld, skinInverse);
						// boneOriginWorld * boneOffsetInverse = boneOffset
						DirectX::XMMATRIX boneOffsetInverse = DirectX::XMMatrixInverse(nullptr, boneOffset);
						// boneNewWorld를 skinMesh의 월드를 기준으로 변환한 로컬좌표
						DirectX::XMMATRIX finalBone = DirectX::XMMatrixMultiply(boneOffsetInverse, boneNewWorld);

						if (_boneTMList.size() < nowMesh->m_BoneList.size())
						{
							DirectX::XMFLOAT4X4 boneTM;
							DirectX::XMStoreFloat4x4(&boneTM, finalBone);

							_boneTMList.emplace_back(boneTM);
						}
						else
						{
							DirectX::XMStoreFloat4x4(&_boneTMList[j], finalBone);
						}
					}
				}
				_fxBoneTransforms->SetMatrixArray(reinterpret_cast<float*>(&_boneTMList[0]), 0, static_cast<uint32_t>(_boneTMList.size()));
			}

			if (nowMesh->m_isBone == false)
			{
				_tech->GetPassByIndex(p)->Apply(0, deviceContext);
				deviceContext->DrawIndexed(nowMesh->m_TotalFaceNum * 3, sumIndexCount, sumVertexCount);

				sumIndexCount += nowMesh->m_TotalFaceNum * 3;
				sumVertexCount += nowMesh->m_TotalVertexNum;
			}
		}
	}
}

void ArkEngine::ArkDX11::ASEMesh::Finalize()
{
	delete _debugObject;

	delete _animator;

	_tech = nullptr;
	_effect = nullptr;

	_vertexBuffer = nullptr;
	_indexBuffer = nullptr;

	_boneTMList.clear();

	_normalMapSRV = nullptr;
	_diffuseMapSRV = nullptr;

	_normalMap->Release();
	_diffuseMap->Release();

	_fxMaterial->Release();
	_fxTexTransform->Release();
	_fxWorldViewProj->Release();
	_fxWorldInvTranspose->Release();
	_fxWorld->Release();

	delete _meshTransform;
	_arkEffect = nullptr;
	_arkDevice = nullptr;


	if (_isStaticMesh == false)
	{
		_fxBoneTransforms->Release();
	}
}

void ArkEngine::ArkDX11::ASEMesh::Delete()
{
	_aseParser->DeleteMeshInfo(_myIndex);
	ResourceManager::GetInstance()->DeleteRenderable(this);
}

bool ArkEngine::ArkDX11::ASEMesh::GetRenderingState()
{
	return _isRendering;
}

void ArkEngine::ArkDX11::ASEMesh::SetRenderingState(bool tf)
{
	_isRendering = tf;

	_debugObject->SetRenderingState(tf);
}

void ArkEngine::ArkDX11::ASEMesh::SetTransform(DirectX::XMFLOAT4X4 matrix)
{
	_meshTransform->SetTransformMatrix(matrix);

	_debugObject->SetTransformMatrix(matrix);
}

void ArkEngine::ArkDX11::ASEMesh::SetPosition(float x, float y, float z)
{
	_meshTransform->SetTranslationMatrix(x, y, z);

	_debugObject->SetPosition(x, y, z);
}

void ArkEngine::ArkDX11::ASEMesh::SetRotation(float x, float y, float z)
{
	_meshTransform->SetRotationMatrix(x, y, z);

	_debugObject->SetRotation(x, y, z);
}

void ArkEngine::ArkDX11::ASEMesh::SetScale(float x, float y, float z)
{
	_meshTransform->SetScaleMatrix(x, y, z);

	_debugObject->SetScale(x, y, z);
}

void ArkEngine::ArkDX11::ASEMesh::SetModel(const char* fileName)
{
	_fileName = fileName;
	
	BuildGeometryBuffers();

	if (_isStaticMesh == false)
	{
		_effectName = "Resources/FX/SkinningDeferred.fx";
	}
}

void ArkEngine::ArkDX11::ASEMesh::SetDiffuseTexture(int index, const char* textureName)
{
	auto texture = ResourceManager::GetInstance()->GetResource<ArkTexture>(textureName);

	if (texture != nullptr)
	{
		_diffuseMapSRV = texture->GetDiffuseMapSRV();
		_tech = _effect->GetTechniqueByName("LightTexReflect");
	}
}

void ArkEngine::ArkDX11::ASEMesh::SetNormalTexture(int index, const char* textureName)
{
	// ASE에는 Tangent값을 구해놓지 않아 일단 노말 텍스쳐는 적용되지 않도록
	//auto texture = ResourceManager::GetInstance()->GetResource<ArkTexture>(textureName);
	//
	//if (texture != nullptr)
	//{
	//	_normalMapSRV = texture->GetDiffuseMapSRV();
	//}
}

void ArkEngine::ArkDX11::ASEMesh::SetEmissiveTexture(int index, const char* textureName)
{

}

void ArkEngine::ArkDX11::ASEMesh::SetAnimator()
{
	if (_isStaticMesh == false)
	{
		_animator = new Animator(_aseParser, _myIndex);
	}
}

const GInterface::Material ArkEngine::ArkDX11::ASEMesh::GetMaterial()
{
	GInterface::Material material;
	
	material.ambient.x = _material.ambient.x;
	material.ambient.y = _material.ambient.y;
	material.ambient.z = _material.ambient.z;
	material.ambient.w = _material.ambient.w;

	material.diffuse.x = _material.diffuse.x;
	material.diffuse.y = _material.diffuse.y;
	material.diffuse.z = _material.diffuse.z;
	material.diffuse.w = _material.diffuse.w;

	material.specular.x = _material.specular.x;
	material.specular.y = _material.specular.y;
	material.specular.z = _material.specular.z;
	material.specular.w = _material.specular.w;

	material.reflect.x = _material.reflect.x;
	material.reflect.y = _material.reflect.y;
	material.reflect.z = _material.reflect.z;
	material.reflect.w = _material.reflect.w;

	return material;
}

void ArkEngine::ArkDX11::ASEMesh::SetMaterial(GInterface::Material material)
{
	_material.ambient = material.ambient;
	_material.diffuse = material.diffuse;
	_material.diffuse = material.diffuse;
	_material.reflect = material.reflect;
}

void ArkEngine::ArkDX11::ASEMesh::SetReflect(DirectX::XMFLOAT4 reflect)
{
	_material.reflect = reflect;
}

const std::vector<std::string> ArkEngine::ArkDX11::ASEMesh::GetDiffuseTextureList()
{
	std::vector<std::string> test;
	return test;
}

const std::vector<std::string> ArkEngine::ArkDX11::ASEMesh::GetNormalTextureList()
{
	std::vector<std::string> test;
	return test;
}

void ArkEngine::ArkDX11::ASEMesh::PlayAnimation(float deltaTime, bool continiousPlay)
{
	if (_animator != nullptr && _isRendering == true)
	{
		if (continiousPlay == true)
		{
			_animator->PlayAnimationContinious(deltaTime);
		}
		else
		{
			_animator->PlayAnimationOnce(deltaTime);
		}
	}
}

void ArkEngine::ArkDX11::ASEMesh::PlayAnimation(float speed, float deltaTime, std::string animName, bool continuousPlay)
{

}

const std::vector<std::string>& ArkEngine::ArkDX11::ASEMesh::GetClipNames()
{
	return std::vector<std::string>();
}

void ArkEngine::ArkDX11::ASEMesh::PauseAnimation()
{

}

void ArkEngine::ArkDX11::ASEMesh::ReplayAnimation()
{

}

float ArkEngine::ArkDX11::ASEMesh::GetCurrentFrame()
{
	return 0;
}

bool ArkEngine::ArkDX11::ASEMesh::GetPickable()
{
	return false;
}

void ArkEngine::ArkDX11::ASEMesh::SetPickable(bool tf)
{

}

unsigned int ArkEngine::ArkDX11::ASEMesh::GetHashID()
{
	return 0;
}

bool ArkEngine::ArkDX11::ASEMesh::GetInsideFrustumState()
{
	return true;
}

void ArkEngine::ArkDX11::ASEMesh::StopAnimation()
{
	if (_animator != nullptr)
	{
		_animator->StopAseAnimation();
	}
}

void ArkEngine::ArkDX11::ASEMesh::PlayAnimation(float speed, float deltaTime, int animIndex, bool couninuousPlay)
{

}

void ArkEngine::ArkDX11::ASEMesh::SetEffect()
{
	_tech = _effect->GetTechniqueByIndex(0);

	if (_isStaticMesh == false)
	{
		_fxBoneTransforms = _effect->GetVariableByName("gBoneTransforms")->AsMatrix();
	}

	_fxWorld = _effect->GetVariableByName("gWorld")->AsMatrix();
	_fxWorldInvTranspose = _effect->GetVariableByName("gWorldInvTranspose")->AsMatrix();
	_fxWorldViewProj = _effect->GetVariableByName("gWorldViewProj")->AsMatrix();
	_fxTexTransform = _effect->GetVariableByName("gTexTransform")->AsMatrix();
	_fxMaterial = _effect->GetVariableByName("gMaterial");

	_diffuseMap = _effect->GetVariableByName("gDiffuseMap")->AsShaderResource();
	_normalMap = _effect->GetVariableByName("gNormalMap")->AsShaderResource();

	_cubeMap = _effect->GetVariableByName("gCubeMap")->AsShaderResource();

	// ase일 경우에 탄젠트값 없기에 픽셸 쉐이더에서 분기 갈라지도록 bool값 보내주기
}

void ArkEngine::ArkDX11::ASEMesh::BuildGeometryBuffers()
{
	_totalVertexCount = 0;
	_totalIndexCount = 0;

	std::vector<Vertex> vertexList;
	std::vector<UINT> indexList;

	//std::filesystem::path _path(파일경로);
	//_path.parent_path().

	DirectX::XMFLOAT3 minPos = { FLT_MAX, FLT_MAX, FLT_MAX };
	DirectX::XMFLOAT3 maxPos = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

	_aseParser = ResourceManager::GetInstance()->GetASEParser(_fileName);

	if (_aseParser == nullptr)
	{
		ASEParser* newParser = new ASEParser();
		newParser->Load(_fileName.c_str());
		ResourceManager::GetInstance()->AddASEParser(_fileName.c_str(), newParser);
		_aseParser = newParser;
	}

	// Dynamic / Static 판단
	if (_aseParser->GetBoneExist())
	{
		_isStaticMesh = false;
	}

	_myIndex = _aseParser->GetMeshCount();

	_aseParser->AddMeshCount();

	auto buffer = ResourceManager::GetInstance()->GetResource<ArkBuffer>(_fileName);

	if (buffer == nullptr)
	{
		for (auto index : _aseParser->GetMeshList())
		{
			if (index->m_isBone == false)
			{
				for (int i = 0; i < index->m_VertexList.size(); i++)
				{
					vertexList.emplace_back();
					vertexList.back().pos = index->m_VertexList[i]->m_Position;
					vertexList.back().tex.x = index->m_VertexList[i]->m_Texture.x;
					vertexList.back().tex.y = index->m_VertexList[i]->m_Texture.y;
					vertexList.back().norm = index->m_VertexList[i]->m_Normal;

					if (_isStaticMesh == false)
					{
						for (int j = 0; j < index->m_VertexList[i]->m_BoneWeightList.size(); j++)
						{
							if (j == 0)
							{
								vertexList.back().boneIndicle.x = index->m_VertexList[i]->m_BoneWeightList[j].m_BoneIndex;
								vertexList.back().weight.x = index->m_VertexList[i]->m_BoneWeightList[j].m_Weight;
							}
							else if (j == 1)
							{
								vertexList.back().boneIndicle.y = index->m_VertexList[i]->m_BoneWeightList[j].m_BoneIndex;
								vertexList.back().weight.y = index->m_VertexList[i]->m_BoneWeightList[j].m_Weight;
							}
							else if (j == 2)
							{
								vertexList.back().boneIndicle.z = index->m_VertexList[i]->m_BoneWeightList[j].m_BoneIndex;
								vertexList.back().weight.z = index->m_VertexList[i]->m_BoneWeightList[j].m_Weight;
							}
							else if (j == 3)
							{
								vertexList.back().boneIndicle.w = index->m_VertexList[i]->m_BoneWeightList[j].m_BoneIndex;
							}
						}
					}
				}

				for (int i = 0; i < index->m_FaceList.size(); i++)
				{
					for (int j = 0; j < 3; j++)
					{
						indexList.emplace_back(index->m_FaceList[i]->m_indexList[j]);
					}
				}

				_totalVertexCount += index->m_TotalVertexNum;
				_totalIndexCount += 3 * (index->m_TotalFaceNum);

				// 최소/최대 위치 찾기
				for (const auto& vertex : vertexList)
				{
					minPos.x = std::min(minPos.x, vertex.pos.x);
					minPos.y = std::min(minPos.y, vertex.pos.y);
					minPos.z = std::min(minPos.z, vertex.pos.z);

					maxPos.x = std::max(maxPos.x, vertex.pos.x);
					maxPos.y = std::max(maxPos.y, vertex.pos.y);
					maxPos.z = std::max(maxPos.z, vertex.pos.z);
				}
			}
		}
		ArkBuffer* newBuffer = new ArkBuffer(_fileName.c_str(), _totalVertexCount, vertexList, _totalIndexCount, indexList);
		buffer = ResourceManager::GetInstance()->GetResource<ArkBuffer>(_fileName);
		buffer->SetSize(minPos, maxPos);
	}

	_vertexBuffer = buffer->GetVertexBuffer();
	_indexBuffer = buffer->GetIndexBuffer();
}

void ArkEngine::ArkDX11::ASEMesh::SetBasicMaterial()
{
	if (_aseParser->GetMaterialList().size() > 0)
	{
		_material.ambient = _aseParser->GetMaterialList().back()->m_ambient;
		_material.diffuse = _aseParser->GetMaterialList().back()->m_diffuse;
		_material.specular = _aseParser->GetMaterialList().back()->m_specular;
	}
	else
	{
		_material.ambient = { 0.0f, 0.0f, 0.0f, 0.0f };
		_material.diffuse = { 0.0f, 0.0f, 0.0f, 0.0f };
		_material.specular = { 0.0f, 0.0f, 0.0f, 0.0f };
	}
}

void ArkEngine::ArkDX11::ASEMesh::SetLight()
{

}

