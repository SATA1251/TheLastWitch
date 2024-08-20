#define NOMINMAX

#include <filesystem>
#include <algorithm>
#include <functional>
#include "ResourceManager.h"
#include "ArkTexture.h"
#include "ArkBuffer.h"
#include "Transform.h"
#include "DirectionalLight.h"
#include "LightManager.h"
#include "GeometryGenerator.h"
#include "DebugObject.h"
#include "ModelAnimation.h"
#include "../FbxLoader/AssimpTool.h"
#include "../FbxLoader/FileUtils.h"
#include "../FbxLoader/Utils.h"
#include "FBXAnimator.h"
#include "DWFont.h"

#include "MeshRenderer.h"

#include "FBXMesh.h"

ArkEngine::ArkDX11::FBXMesh::FBXMesh(const std::string& fileName, bool isSolid)
	: _simpleModelName(), _fileName(fileName), _effectName("Resources/FX/BasicTexDeferred.fx"),
	_diffuseTextureName(), _normalTextureName(),
	_diffuseMapSRV(), _normalMapSRV(), _emissionMapSRV(),
	_boneTMList(), _vertexBuffer(), _indexBuffer(),
	_totalVertexCount(0), _totalIndexCount(0), _meshTransform(nullptr),
	_debugObject(nullptr),
	_havePlayedAnimation(false), _isAnimationPlaying(false), _isRendering(true), _isSolid(isSolid),
	_isPickable(false), _hashValue(0), _objectIndex(0), _color(), _haveShadow(true), _applyCartoonRendering(false),
	_modelPath(L"Resources/Models/"), _texturePath(L"Resources/Textures/"),
	_meshCount(0), _boneIndexNum(0),
	_newVertexVector(0), _newIndexVector(0), _boneTransforms(0),
	_animator(nullptr),
	_parentMesh(nullptr), _parentBoneIndex(0), _parentBoneTrasnform(), _transformEffectedByParent(), 
	_alpha(1.0f),
	_isDissolve(false), _dissolveValue(1.0f)
{
	Initialize();
}

ArkEngine::ArkDX11::FBXMesh::~FBXMesh()
{
	Finalize();
}

void ArkEngine::ArkDX11::FBXMesh::Initialize()
{
	size_t pos = _fileName.find_last_of("/");

	if (pos != std::string::npos) {
		_simpleModelName = _fileName.substr(0, pos);
	}

	_objectIndex = ResourceManager::GetInstance()->GetObjectIndex();
	for (int i = 0; i < 4; i++)
	{
		_color[i] = 0.0f;
	}

	// 애니메이터를 먼저 초기화 해줘야 애니메이션 데이터들을 읽어올 수가 있는데
	// isStatic으로 구분을 지을 수가 없이 계속 생성을 해준다
	// 이쪽은 해결해줘야 할 문제임
	SetAnimator();

	std::unique_ptr<ArkEngine::Utils> utils;

	utils = std::make_unique<ArkEngine::Utils>();

	if (_animator->CheckClipFile(_fileName) == false)
	{
		_animator = nullptr;
	}

	if (_animator != nullptr)
	{
		_effectName = "Resources/FX/SkinningDeferred.fx";
	}

	// 이쪽에서 무슨 모델 띄울건지 정해준다
	BuildGeometryBuffersFBX(utils->ToWString(_fileName));
	ReadMaterial(utils->ToWString(_fileName));
	//if (_diffuseTextureName.back().find("png") == std::string::npos)
	//{
	//	SetDiffuseTexture(_meshes.size()-1, "Resources/Textures/fox.dds");
	//}

	_meshTransform = new Transform();

	// 그림자 맵용 추가
	//_shadowMap = new ShadowMap(_arkDevice->GetDevice(), 2048, 2048);
	//_shadowMap->BindDsvAndSetNullRenderTarget(_arkDevice->GetDeviceContext());


	_debugObject = new DebugObject(_fileName, DebugObject::eDebugType::Cube);

	auto meshRenderer = ResourceManager::GetInstance()->GetMeshRenderer(_fileName);

	if (meshRenderer == nullptr)
	{
		auto mesh = new MeshRenderer(this);
	}
	else
	{
		meshRenderer->AddMeshInList(this);
	}
}

void ArkEngine::ArkDX11::FBXMesh::Update(ArkEngine::ICamera* p_Camera)
{

}

void ArkEngine::ArkDX11::FBXMesh::Render()
{

}

void ArkEngine::ArkDX11::FBXMesh::Render(int passIndex)
{
	
}

void ArkEngine::ArkDX11::FBXMesh::Finalize()
{
	auto meshRenderer = ResourceManager::GetInstance()->GetMeshRenderer(_fileName);

	meshRenderer->DeleteMeshInList(this);

	_parentMesh = nullptr;

	delete _debugObject;

	for (int i = 0; i < _vertexBuffer.size(); i++)
	{
		_vertexBuffer[i] = nullptr;
	}

	for (int i = 0; i < _indexBuffer.size(); i++)
	{
		_indexBuffer[i] = nullptr;
	}

	_boneTMList.clear();

	_emissionMapSRV.clear();
	_normalMapSRV.clear();
	_diffuseMapSRV.clear();

	delete _animator;

	delete _meshTransform;
}

void ArkEngine::ArkDX11::FBXMesh::Delete()
{
	ResourceManager::GetInstance()->DeleteRenderable(this);
}

bool ArkEngine::ArkDX11::FBXMesh::GetRenderingState()
{
	return _isRendering;
}

void ArkEngine::ArkDX11::FBXMesh::SetRenderingState(bool tf)
{
	auto meshRenderer = ResourceManager::GetInstance()->GetMeshRenderer(_fileName);
	
	meshRenderer->DeleteMeshInList(this);
	
	if (tf == true)
	{
		meshRenderer->AddMeshInList(this);
	}

	_isRendering = tf;

	_debugObject->SetActive(tf);
}

void ArkEngine::ArkDX11::FBXMesh::SetShadowState(bool tf)
{
	_haveShadow = tf;
}

bool ArkEngine::ArkDX11::FBXMesh::GetShadowState()
{
	return _haveShadow;
}

void ArkEngine::ArkDX11::FBXMesh::SetTransform(const DirectX::XMFLOAT4X4& matrix)
{
	_meshTransform->SetTransformMatrix(matrix);

	if (_parentMesh == nullptr)
	{
		_debugObject->SetTransformMatrix(matrix);
	}
}

void ArkEngine::ArkDX11::FBXMesh::SetPosition(float x, float y, float z)
{
	_meshTransform->SetTranslationMatrix(x, y, z);

	_debugObject->SetPosition(x, y, z);
}

void ArkEngine::ArkDX11::FBXMesh::SetRotation(float x, float y, float z)
{
	_meshTransform->SetRotationMatrix(x, y, z);

	_debugObject->SetRotation(x, y, z);
}

void ArkEngine::ArkDX11::FBXMesh::SetScale(float x, float y, float z)
{
	_meshTransform->SetScaleMatrix(x, y, z);

	_debugObject->SetScale(x, y, z);
}

void ArkEngine::ArkDX11::FBXMesh::SetModel(const char* fileName)
{
	_fileName = fileName;
}

void ArkEngine::ArkDX11::FBXMesh::SetDiffuseTexture(int index, const char* textureName)
{
	if (index < _diffuseTextureName.size())
	{
		std::string finalTextureName = textureName;
		if (finalTextureName.find("Resources/Textures/") == std::string::npos)
		{
			finalTextureName = "Resources/Textures/" + finalTextureName;
		}

		_diffuseTextureName[index] = finalTextureName.c_str();

		auto texture = ResourceManager::GetInstance()->GetResource<ArkTexture>(_diffuseTextureName[index]);

		if (texture != nullptr)
		{
			_diffuseMapSRV[index] = texture->GetDiffuseMapSRV();
		}
		else
		{
			ArkTexture* newTexture = new ArkTexture(finalTextureName.c_str());
			_diffuseMapSRV[index] = newTexture->GetDiffuseMapSRV();
		}
	}
}

void ArkEngine::ArkDX11::FBXMesh::SetNormalTexture(int index, const char* textureName)
{
	if (index < _normalTextureName.size())
	{
		std::string finalTextureName = textureName;
		if (finalTextureName.find("Resources/Textures/") == std::string::npos)
		{

			finalTextureName = "Resources/Textures/" + finalTextureName;
		}

		_normalTextureName[index] = finalTextureName.c_str();


		auto texture = ResourceManager::GetInstance()->GetResource<ArkTexture>(_normalTextureName[index]);

		if (texture != nullptr)
		{
			_normalMapSRV[index] = texture->GetDiffuseMapSRV();
		}
		else
		{
			ArkTexture* newTexture = new ArkTexture(finalTextureName.c_str());
			_normalMapSRV[index] = newTexture->GetDiffuseMapSRV();
		}
	}
}

void ArkEngine::ArkDX11::FBXMesh::SetEmissiveTexture(int index, const char* textureName)
{
	if (index < _emssiveTextureName.size())
	{
		std::string finalTextureName = textureName;
		if (finalTextureName.find("Resources/Textures/") == std::string::npos)
		{

			finalTextureName = "Resources/Textures/" + finalTextureName;
		}

		_emssiveTextureName[index] = finalTextureName.c_str();


		auto texture = ResourceManager::GetInstance()->GetResource<ArkTexture>(_emssiveTextureName[index]);

		if (texture != nullptr)
		{
			_emissionMapSRV[index] = texture->GetDiffuseMapSRV();
		}
		else
		{
			ArkTexture* newTexture = new ArkTexture(finalTextureName.c_str());

			_emissionMapSRV[index] = newTexture->GetDiffuseMapSRV();
		}
	}
}

void ArkEngine::ArkDX11::FBXMesh::SetAnimator()
{
	_animator = new FBXAnimator();
}

const GInterface::Material ArkEngine::ArkDX11::FBXMesh::GetMaterial()
{
	GInterface::Material material;

	material.ambient.x = _material.back().ambient.x;
	material.ambient.y = _material.back().ambient.y;
	material.ambient.z = _material.back().ambient.z;
	material.ambient.w = _material.back().ambient.w;

	material.diffuse.x = _material.back().diffuse.x;
	material.diffuse.y = _material.back().diffuse.y;
	material.diffuse.z = _material.back().diffuse.z;
	material.diffuse.w = _material.back().diffuse.w;

	material.specular.x = _material.back().specular.x;
	material.specular.y = _material.back().specular.y;
	material.specular.z = _material.back().specular.z;
	material.specular.w = _material.back().specular.w;

	material.reflect.x = _material.back().reflect.x;
	material.reflect.y = _material.back().reflect.y;
	material.reflect.z = _material.back().reflect.z;
	material.reflect.w = _material.back().reflect.w;

	return material;
}

void ArkEngine::ArkDX11::FBXMesh::SetMaterial(GInterface::Material material)
{
	Material newMat;

	newMat.ambient = material.ambient;
	newMat.diffuse = material.diffuse;
	newMat.diffuse = material.diffuse;
	newMat.reflect = material.reflect;

	_material.emplace_back(newMat);
}

void ArkEngine::ArkDX11::FBXMesh::SetReflect(const DirectX::XMFLOAT4& reflect)
{
	_material.back().reflect = reflect;
}

const std::vector<std::string> ArkEngine::ArkDX11::FBXMesh::GetDiffuseTextureList()
{
	return _diffuseTextureName;
}

const std::vector<std::string> ArkEngine::ArkDX11::FBXMesh::GetNormalTextureList()
{
	return _normalTextureName;
}

void ArkEngine::ArkDX11::FBXMesh::PlayAnimation(float deltaTime, bool continiousPlay)
{
}

const std::vector<std::string>& ArkEngine::ArkDX11::FBXMesh::GetClipNames()
{
	if (_animator != nullptr)
	{
		return _animator->GetClipNames();
	}
}

void ArkEngine::ArkDX11::FBXMesh::PauseAnimation()
{
	if (_animator != nullptr)
	{
		_animator->PauseFBXAnimation();
	}
}

void ArkEngine::ArkDX11::FBXMesh::ReplayAnimation()
{
	if (_animator != nullptr)
	{
		_animator->RestartFBXAnimation();
	}
}

float ArkEngine::ArkDX11::FBXMesh::GetCurrentFrame()
{
	if (_animator != nullptr)
	{
		return _animator->GetCurrentFrame();
	}
}


void ArkEngine::ArkDX11::FBXMesh::SetCurrentFrame(int frame)
{
	return _animator->SetCurrentFrame(frame);
}

float ArkEngine::ArkDX11::FBXMesh::GetMaxFrame()
{
	return _animator->GetMaxFrame();
}

bool ArkEngine::ArkDX11::FBXMesh::GetIsPlaying()
{
	return _isAnimationPlaying;
}

const std::string& ArkEngine::ArkDX11::FBXMesh::GetNowAnimationName()
{
	return _animator->GetAnimationName();
}

void ArkEngine::ArkDX11::FBXMesh::SetParentBone(GInterface::GraphicsRenderable* model, const std::string& boneName)
{
	_parentMesh = dynamic_cast<FBXMesh*>(model);

	_parentBoneIndex = _parentMesh->_animator->GetIndexOfBone(boneName);

	DirectX::XMMATRIX boneTransform;
	if (_parentMesh != nullptr)
	{
		auto boneTransform = _parentMesh->_animator->GetBoneTransform(_parentBoneIndex);

		_parentBoneTrasnform = boneTransform;
	}
}

void ArkEngine::ArkDX11::FBXMesh::DeleteParentBone()
{
	_parentMesh = nullptr;
	DirectX::XMStoreFloat4x4(&_parentBoneTrasnform, DirectX::XMMatrixIdentity());
	_parentBoneIndex = 0;
}

DirectX::XMFLOAT4X4 ArkEngine::ArkDX11::FBXMesh::GetTransformEffectedByBone()
{
	return _transformEffectedByParent;
}

DirectX::XMFLOAT4X4 ArkEngine::ArkDX11::FBXMesh::GetBoneTransform(const std::string& boneName)
{
	auto boneIndex = _animator->GetIndexOfBone(boneName);

	auto boneOriginWorld = _animator->GetBoneTransform(boneIndex);
	auto boneAnim = _animator->GetBoneAnimation(boneIndex);

	auto transform = DirectX::XMMatrixMultiply(DirectX::XMLoadFloat4x4(&boneOriginWorld), DirectX::XMLoadFloat4x4(&boneAnim));
	auto toParent = DirectX::XMMatrixMultiply(transform, GetWorldTransform());

	DirectX::XMFLOAT4X4 result;
	DirectX::XMStoreFloat4x4(&result, toParent);

	return result;
}

void ArkEngine::ArkDX11::FBXMesh::StopAnimation()
{
	_isAnimationPlaying = false;

	if (_animator != nullptr)
	{
		_animator->StopFBXAnimation();
	}
}

bool ArkEngine::ArkDX11::FBXMesh::PlayAnimation(float speed, float deltaTime, int animIndex, bool continuousPlay)
{
	_havePlayedAnimation = true;

	if (_animator != nullptr)
	{
		if (continuousPlay == true)
		{
			_isAnimationPlaying = _animator->PlayAnimationContinuous(speed, deltaTime, animIndex);
			return _isAnimationPlaying;
		}
		else
		{
			_isAnimationPlaying = _animator->PlayAnimationOnce(speed, deltaTime, animIndex);
			return _isAnimationPlaying;
		}
	}
}

bool ArkEngine::ArkDX11::FBXMesh::PlayAnimation(float speed, float deltaTime, const std::string& animName, bool continuousPlay)
{
	_havePlayedAnimation = true;

	if (_animator != nullptr)
	{
		if (continuousPlay == true)
		{
			_isAnimationPlaying = _animator->PlayAnimationContinuous(speed, deltaTime, animName);
			return _isAnimationPlaying;
		}
		else
		{
			_isAnimationPlaying = _animator->PlayAnimationOnce(speed, deltaTime, animName);
			return _isAnimationPlaying;
		}
	}
}

bool ArkEngine::ArkDX11::FBXMesh::GetPickable()
{
	return _isPickable;
}

void ArkEngine::ArkDX11::FBXMesh::SetPickable(bool tf)
{
	if (tf)
	{
		SetHashValue(_objectIndex);
	}
	else
	{
		_hashValue = 0;
		for (int i = 0; i < 4; i++)
		{
			_color[i] = 0.0f;
		}
	}

	_isPickable = tf;
}

unsigned int ArkEngine::ArkDX11::FBXMesh::GetHashID()
{
	return _hashValue;
}

bool ArkEngine::ArkDX11::FBXMesh::GetInsideFrustumState()
{
	return _debugObject->GetRenderingState();
}

void ArkEngine::ArkDX11::FBXMesh::SetHashValue(unsigned int index)
{
	if (_hashValue == 0)
	{
		std::hash<int> intHash;

		_hashValue = intHash(index);
		ConvertHashToRGBA(_hashValue);
	}
}

void ArkEngine::ArkDX11::FBXMesh::ConvertHashToRGBA(int hashValue)
{
	float a = (hashValue >> 24) & 0xff;
	float b = (hashValue >> 16) & 0xff;
	float g = (hashValue >> 8) & 0xff;
	float r = hashValue & 0xff;

	_color[0] = r / 255.0f;
	_color[1] = g / 255.0f;
	_color[2] = b / 255.0f;
	_color[3] = a / 255.0f;

	_colorVec.x = _color[0];
	_colorVec.y = _color[1];
	_colorVec.z = _color[2];
	_colorVec.w = _color[3];
}

const DirectX::XMMATRIX ArkEngine::ArkDX11::FBXMesh::GetWorldTransform()
{
	return _meshTransform->GetTransformMatrix();
}

const std::string& ArkEngine::ArkDX11::FBXMesh::GetFileName()
{
	return _fileName;
}

void ArkEngine::ArkDX11::FBXMesh::SetMainCamera(ArkEngine::ICamera* camera)
{
}


void ArkEngine::ArkDX11::FBXMesh::SetCartoonRendering(bool tf)
{
	_applyCartoonRendering = tf;
}


bool ArkEngine::ArkDX11::FBXMesh::GetCartoonRenderingState()
{
	return _applyCartoonRendering;
}


void ArkEngine::ArkDX11::FBXMesh::SetAlpha(float alpha)
{
	_alpha = alpha;
}


float ArkEngine::ArkDX11::FBXMesh::GetAlhpa()
{
	return _alpha;
}


const float ArkEngine::ArkDX11::FBXMesh::GetDepth(DirectX::XMFLOAT3& cameraPos)
{
	DirectX::XMVECTOR source = DirectX::XMLoadFloat3(&cameraPos);

	DirectX::XMVECTOR distance = DirectX::XMVectorSubtract(_meshTransform->GetTransformMatrix().r[3], source);

	float result = DirectX::XMVectorGetX(DirectX::XMVector3Length(distance));

	return result;
}

const std::string& ArkEngine::ArkDX11::FBXMesh::GetName()
{
	return _fileName;
}


const std::string& ArkEngine::ArkDX11::FBXMesh::GetEffectName()
{
	return _effectName;
}


std::vector<ID3D11ShaderResourceView*>& ArkEngine::ArkDX11::FBXMesh::GetDiffuseSRV()
{
	return _diffuseMapSRV;
}


std::vector<ID3D11ShaderResourceView*>& ArkEngine::ArkDX11::FBXMesh::GetNormalSRV()
{
	return _normalMapSRV;
}


std::vector<ID3D11ShaderResourceView*>& ArkEngine::ArkDX11::FBXMesh::GetEmmisionSRV()
{
	return _emissionMapSRV;
}


ArkEngine::ArkDX11::FBXAnimator* ArkEngine::ArkDX11::FBXMesh::GetAnimator()
{
	return _animator;
}


const DirectX::XMMATRIX ArkEngine::ArkDX11::FBXMesh::GetTransformMat()
{
	return _meshTransform->GetTransformMatrix();
}


ArkEngine::ArkDX11::FBXMesh* ArkEngine::ArkDX11::FBXMesh::GetParentMesh()
{
	return _parentMesh;
}

unsigned int ArkEngine::ArkDX11::FBXMesh::GetParentBoneIndex()
{
	return _parentBoneIndex;
}

DirectX::XMFLOAT4X4 ArkEngine::ArkDX11::FBXMesh::GetParentBoneOriginalTransform(const std::string& boneName)
{
	auto parentOriginWorld = GetParentBoneTransform();
	auto parentBoneAnim = _parentMesh->GetAnimator()->GetBoneAnimation(_parentBoneIndex);
	auto transform = DirectX::XMMatrixMultiply(parentOriginWorld, DirectX::XMLoadFloat4x4(&parentBoneAnim));
	auto world = _parentMesh->GetWorldTransform();
	transform = DirectX::XMMatrixMultiply(transform, world);

	DirectX::XMFLOAT4X4 result;
	DirectX::XMStoreFloat4x4(&result, transform);

	return result;
}

DirectX::XMMATRIX ArkEngine::ArkDX11::FBXMesh::GetParentBoneTransform()
{
	auto trs = DirectX::XMLoadFloat4x4(&_parentBoneTrasnform);
	return trs;
}


const DirectX::XMFLOAT4 ArkEngine::ArkDX11::FBXMesh::GetColor()
{
	return _colorVec;
}

/// <summary>
/// FBX용 추가한것
/// </summary>
void ArkEngine::ArkDX11::FBXMesh::BuildGeometryBuffersFBX(std::wstring fileName)
{
	auto buffer = ResourceManager::GetInstance()->GetResource<ArkBuffer>(_fileName);

	if (buffer == nullptr)
	{
		auto modelMesh = new ModelMesh();

		std::vector<ModelMesh*> meshes;

		meshes.emplace_back(modelMesh);

		DirectX::XMFLOAT3 minPos = { FLT_MAX, FLT_MAX, FLT_MAX };
		DirectX::XMFLOAT3 maxPos = { -FLT_MAX, -FLT_MAX, -FLT_MAX };
		
		std::wstring fullPath = _modelPath + fileName + L".mesh";

		std::unique_ptr<ArkEngine::FileUtils> file = std::make_unique<ArkEngine::FileUtils>();

		std::unique_ptr<ArkEngine::Utils> utils;

		utils = std::make_unique<ArkEngine::Utils>();

		file->Open(fullPath, FileMode::Read);

		//Bones
		UINT boneCount = file->Read<UINT>();

		for (UINT i = 0; i < boneCount; i++)
		{
			modelMesh->bone = std::make_shared<ModelBone>();
			modelMesh->bone->index = file->Read<int>();
			modelMesh->bone->name = utils->ToWString(file->Read<std::string>());
			modelMesh->bone->parentIndex = file->Read<int>();
			modelMesh->bone->transform = file->Read<Matrix>();

			// offsetTM 추가 값들이 잘 들어가나 봐야한다
			modelMesh->bone->boneOffsetTM = file->Read<Matrix>();
			modelMesh->_boneList.emplace_back(modelMesh->bone);
		}

		// Mesh
		{
			_meshCount = file->Read<UINT>();

			for (int i = 0; i < _meshCount - 1; i++)
			{
				meshes.emplace_back(new ModelMesh());
			}

			// 메쉬 수 만큼 반복시행
			for (UINT i = 0; i < _meshCount; i++)
			{
				auto modelMesh = meshes[i];

				modelMesh->name = utils->ToWString(file->Read<std::string>());
				modelMesh->boneIndex = file->Read<UINT>();

				// Material
				modelMesh->materialName = utils->ToWString(file->Read<std::string>());

				/// Vertex Data
				modelMesh->vertexNum = file->Read<UINT>();

				// vertex의 개수에 따라 vector의 크기를 조정
				modelMesh->vertices.resize(modelMesh->vertexNum);

				// 컨테이너에 담신 첫번째 요소를 가리킴
				void* vertexData = modelMesh->vertices.data();

				// 데이터들을 읽어온다
				file->Read(&vertexData, sizeof(ModelVertexType) * modelMesh->vertexNum);

				/// Index Data
				modelMesh->indexNum = file->Read<UINT>();

				modelMesh->indices.resize(modelMesh->indexNum);

				void* indexData = modelMesh->indices.data();

				file->Read(&indexData, sizeof(UINT) * modelMesh->indexNum);


				_newVertexVector.emplace_back();
				_newIndexVector.emplace_back();
				// FBXLoader에서 사용하던 구조체를 종화그래픽스엔진의 구조체에 맞게 집어 넣어 준다
				for (UINT j = 0; j < modelMesh->vertexNum; j++)
				{
					ArkEngine::ArkDX11::Vertex vertex;

					vertex.pos = modelMesh->vertices[j].position;

					//if (_fileName.find("PlayerWithCloak") != std::string::npos)
					//{
					//	vertex.norm.x = -1* modelMesh->vertices[j].normal.x;
					//	vertex.norm.y = -1* modelMesh->vertices[j].normal.y;
					//	vertex.norm.z = -1* modelMesh->vertices[j].normal.z;
					//}
					//else
					{
						vertex.norm.x = modelMesh->vertices[j].normal.x;
						vertex.norm.y = modelMesh->vertices[j].normal.y;
						vertex.norm.z = modelMesh->vertices[j].normal.z;
					}

					vertex.tex.x = modelMesh->vertices[j].uv.x;
					vertex.tex.y = modelMesh->vertices[j].uv.y;

					vertex.tangent.x = modelMesh->vertices[j].tangent.x;
					vertex.tangent.y = modelMesh->vertices[j].tangent.y;
					vertex.tangent.z = modelMesh->vertices[j].tangent.z;

					vertex.weight = modelMesh->vertices[j].weight;
					vertex.boneIndicle = modelMesh->vertices[j].boneIndices;

					_newVertexVector.back().emplace_back(vertex);
				}

				for (UINT j = 0; j < modelMesh->indexNum; j++)
				{
					_newIndexVector.back().emplace_back(modelMesh->indices[j]);
				}

				meshes[i]->vertexNum = modelMesh->vertexNum;
				meshes[i]->indexNum = modelMesh->indexNum;

				_totalVertexCount += modelMesh->vertexNum;
				_totalIndexCount += modelMesh->indexNum;

				// 최소/최대 위치 찾기
				for (const auto& vertex : modelMesh->vertices)
				{
					minPos.x = std::min(minPos.x, vertex.position.x);
					minPos.y = std::min(minPos.y, vertex.position.y);
					minPos.z = std::min(minPos.z, vertex.position.z);

					maxPos.x = std::max(maxPos.x, vertex.position.x);
					maxPos.y = std::max(maxPos.y, vertex.position.y);
					maxPos.z = std::max(maxPos.z, vertex.position.z);
				}

				ArkBuffer* newBuffer = new ArkBuffer(_fileName.c_str(), modelMesh->vertexNum, _newVertexVector[i], modelMesh->indexNum, _newIndexVector[i]);
				newBuffer->SetSize(minPos, maxPos);

				ResourceManager::GetInstance()->AddFbxParsingData(_fileName, meshes);

				ResourceManager::GetInstance()->AddArkBuffer(_fileName, newBuffer);
			}
		}
	}

	_meshes = ResourceManager::GetInstance()->GetFbxParsingData(_fileName);

	for (auto index : ResourceManager::GetInstance()->GetArkBuffer(_fileName))
	{
		_vertexBuffer.emplace_back(index->GetVertexBuffer());
		_indexBuffer.emplace_back(index->GetIndexBuffer());
	}

	if (_animator != nullptr)
	{
		_animator->_animBones = _meshes[0]->_boneList;

		BindCacheInfo();
	}
}

void ArkEngine::ArkDX11::FBXMesh::BindCacheInfo()
{
	// Mesh에 Bone 캐싱
	for (const auto& mesh : _meshes)
	{
		// 이미 찾았으면 스킵
		if (mesh->bone != nullptr)
			continue;
	}

	for (auto index : _meshes)
	{
		if (index->bone == nullptr && _animator->_animBones.size() > 0)
		{
			index->bone = _animator->_animBones[0];

			for (const auto& bone : _animator->_animBones)
			{
				if (bone->parentIndex >= 0)
				{
					bone->parent = _animator->_animBones[bone->parentIndex];
					bone->parent->children.push_back(bone);
				}
				else
				{
					bone->parent = nullptr;
				}
			}
		}
	}
}

void ArkEngine::ArkDX11::FBXMesh::ReadMaterial(std::wstring fileName)
{
	_diffuseTextureName.resize(_meshes.size());
	_diffuseMapSRV.resize(_meshes.size());
	_normalTextureName.resize(_meshes.size());
	_normalMapSRV.resize(_meshes.size());
	_emssiveTextureName.resize(_meshes.size());
	_emissionMapSRV.resize(_meshes.size());

	auto modelMaterialData = ResourceManager::GetInstance()->GetModelMaterial(fileName);

	if (modelMaterialData == nullptr)
	{
		ModelMaterial* materialData = new ModelMaterial();

		std::unique_ptr<ArkEngine::Utils> utils;

		std::wstring fullPath = _texturePath + fileName + L".xml";

		auto parentPath = std::filesystem::path(fullPath).parent_path();

		// 파일 경로를 std::string으로 변환
		std::string utf8Path(fullPath.begin(), fullPath.end());

		// XMLDocument 객체 생성
		tinyxml2::XMLDocument document;

		// XML 파일 로드
		if (document.LoadFile(utf8Path.c_str()) == tinyxml2::XML_SUCCESS)
		{
			materialData->_diffuseTextureList.resize(_meshes.size());
			materialData->_normalTextureList.resize(_meshes.size());
			materialData->_emissiveTextureList.resize(_meshes.size());

			// Materials 엘리먼트 가져오기
			tinyxml2::XMLElement* materialsElement = document.FirstChildElement("Materials");

			if (materialsElement)
			{
				// Material 엘리먼트들 순회
				int meshIndex = 0;
				for (tinyxml2::XMLElement* materialElement = materialsElement->FirstChildElement("Material");
					materialElement; materialElement = materialElement->NextSiblingElement("Material"))
				{
					// diffuse 텍스쳐
					const tinyxml2::XMLElement* diffuseFileElement = materialElement->FirstChildElement("DiffuseFile");
					if (diffuseFileElement)
					{
						const char* diffuseFileText = diffuseFileElement->GetText();

						if (diffuseFileText)
						{
							materialData->_diffuseTextureList[meshIndex] = diffuseFileText;
						}
					}
					// normal 텍스쳐
					const tinyxml2::XMLElement* normalFileElement = materialElement->FirstChildElement("NormalFile");
					if (normalFileElement)
					{
						const char* normalFileText = normalFileElement->GetText();

						if (normalFileText)
						{
							materialData->_normalTextureList[meshIndex] = normalFileText;
						}
					}

					// emissive 텍스쳐
					const tinyxml2::XMLElement* emissiveFileElement = materialElement->FirstChildElement("EmissiveFile");
					if (emissiveFileElement)
					{
						const char* emissiveFileText = emissiveFileElement->GetText();

						if (emissiveFileText)
						{
							materialData->_emissiveTextureList[meshIndex] = emissiveFileText;
						}
					}

					meshIndex++;

					// Material 엘리먼트의 하위 엘리먼트들에서 데이터 추출
					Material material;

					material.ambient.x = materialElement->FirstChildElement("Ambient")->FloatAttribute("R");
					material.ambient.y = materialElement->FirstChildElement("Ambient")->FloatAttribute("G");
					material.ambient.z = materialElement->FirstChildElement("Ambient")->FloatAttribute("B");
					material.ambient.w = materialElement->FirstChildElement("Ambient")->FloatAttribute("A");

					material.diffuse.x = materialElement->FirstChildElement("Diffuse")->FloatAttribute("R");
					material.diffuse.y = materialElement->FirstChildElement("Diffuse")->FloatAttribute("G");
					material.diffuse.z = materialElement->FirstChildElement("Diffuse")->FloatAttribute("B");
					material.diffuse.w = materialElement->FirstChildElement("Diffuse")->FloatAttribute("A");

					material.specular.x = materialElement->FirstChildElement("Specular")->FloatAttribute("R");
					material.specular.y = materialElement->FirstChildElement("Specular")->FloatAttribute("G");
					material.specular.z = materialElement->FirstChildElement("Specular")->FloatAttribute("B");
					material.specular.w = materialElement->FirstChildElement("Specular")->FloatAttribute("A");

					material.reflect.x = materialElement->FirstChildElement("Emissive")->FloatAttribute("R");
					material.reflect.y = materialElement->FirstChildElement("Emissive")->FloatAttribute("G");
					material.reflect.z = materialElement->FirstChildElement("Emissive")->FloatAttribute("B");
					material.reflect.w = materialElement->FirstChildElement("Emissive")->FloatAttribute("A");

					// 추출한 데이터를 materials 벡터에 추가
					materialData->_material.emplace_back(material);
				}
			}
			else
			{
				std::cerr << "Error: 'Materials' element not found in the XML file." << std::endl;
			}
		}
		else
		{
			std::cerr << "Error: Failed to load XML file." << std::endl;
		}

		ResourceManager::GetInstance()->AddModelMaterial(fileName, materialData);

		modelMaterialData = materialData;
	}
	
	for (int index = 0; index < _meshes.size(); index++)
	{
		if (!modelMaterialData->_diffuseTextureList.empty())
		{
			std::string diffuseTexturePath = _simpleModelName + "/" + modelMaterialData->_diffuseTextureList[index];
			SetDiffuseTexture(index, diffuseTexturePath.c_str());
		}

		if (!modelMaterialData->_normalTextureList.empty())
		{
			std::string normalTexturePath = _simpleModelName + "/" + modelMaterialData->_normalTextureList[index];
			SetNormalTexture(index, normalTexturePath.c_str());
		}

		if (!modelMaterialData->_emissiveTextureList.empty())
		{
			std::string emissiveTexturePath = _simpleModelName + "/" + modelMaterialData->_emissiveTextureList[index];
			SetEmissiveTexture(index, emissiveTexturePath.c_str());
		}
	}

	_material = modelMaterialData->_material;
}

void ArkEngine::ArkDX11::FBXMesh::SetDissolve(float value)
{
	_dissolveValue = value;
}

float ArkEngine::ArkDX11::FBXMesh::GetDissolveValue()
{
	return _dissolveValue;
}

void ArkEngine::ArkDX11::FBXMesh::SetIsDissolve(bool isDissolve)
{
	_isDissolve = isDissolve;
}

bool ArkEngine::ArkDX11::FBXMesh::GetIsDissolve()
{
	return _isDissolve;
}

