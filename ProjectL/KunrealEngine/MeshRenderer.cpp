#include "MeshRenderer.h"
#include "GameObject.h"
#include "Transform.h"
#include "Animator.h"

KunrealEngine::MeshRenderer::MeshRenderer()
	:_mesh(nullptr), _transform(nullptr), _reflect(0.f, 0.f, 0.f, 0.f), _isPickable(false)
	, _meshFileName(""), _textureName(""), _normalName(""), _isShadow(true), _isCartoon(false)
{

}

KunrealEngine::MeshRenderer::~MeshRenderer()
{

}

void KunrealEngine::MeshRenderer::Initialize()
{
	this->_transform = this->GetOwner()->GetComponent<Transform>();
}

void KunrealEngine::MeshRenderer::Release()
{
	if (GetMeshStatus())
	{
		this->_mesh->Delete();
	}
}

void KunrealEngine::MeshRenderer::FixedUpdate()
{

}

void KunrealEngine::MeshRenderer::Update()
{
	if (GetMeshStatus())
	{
		this->_mesh->SetTransform(_transform->GetWorldTM());
	}

	if (_transform->_haveParentBone)
	{
		if (_mesh != nullptr)
		{
			DirectX::XMFLOAT4X4 worldFromGraphics = this->_mesh->GetTransformEffectedByBone();
			DirectX::XMMATRIX worldTM = DirectX::XMLoadFloat4x4(&worldFromGraphics);

			DirectX::XMVECTOR scale;
			DirectX::XMVECTOR quat;
			DirectX::XMVECTOR translation;

			DirectX::XMMatrixDecompose(&scale, &quat, &translation, worldTM);

			DirectX::XMStoreFloat3(&this->_transform->_posForBone, translation);
			DirectX::XMStoreFloat4(&this->_transform->_quatForBone, quat);
		}
	}
}

void KunrealEngine::MeshRenderer::LateUpdate()
{

}  

void KunrealEngine::MeshRenderer::OnTriggerEnter()
{

}

void KunrealEngine::MeshRenderer::OnTriggerStay()
{

}

void KunrealEngine::MeshRenderer::OnTriggerExit()
{

}

void KunrealEngine::MeshRenderer::SetActive(bool active)
{
	if (_mesh != nullptr)
	{
		SetRenderingState(active);
	}

	this->_isActivated = active;
}

void KunrealEngine::MeshRenderer::SetMeshObject(const char* fileName, bool isSolid /*= true*/)
{
	this->_mesh = GRAPHICS->CreateRenderable(fileName, isSolid);
	this->_meshFileName = fileName;

	this->GetOwner()->AddComponentParam<Animator>(_mesh);

	// 그림자는 기본적으로 표시해줘야 하는데, 컴포넌트에서 false로 따로 지정해준게 아니라면 그래픽스에 그림자 그리라고 전달
	if (this->_isShadow)
	{
		SetShadowState(true);
	}
}

GInterface::GraphicsRenderable* KunrealEngine::MeshRenderer::GetMeshObject()
{
	if (this->_mesh != nullptr)
	{
		return this->_mesh;
	}
}

std::string KunrealEngine::MeshRenderer::GetMeshName()
{
	if (_mesh != nullptr)
	{
		return this->_meshFileName;
	}
}

void KunrealEngine::MeshRenderer::ChangeMeshObject(const char* fileName)
{
	this->_mesh->Delete();
	
	SetMeshObject(fileName);
}

void KunrealEngine::MeshRenderer::SetPickableState(bool param)
{
	if (param)
	{
		// 이미 true인데 다시 true를 입력했을 경우엔 아무것도 안함
		if (_isPickable)
		{
			return;
		}
		// 그래픽스 시스템의 컨테이너에 추가
		else
		{
			this->_mesh->SetPickable(param);
			GraphicsSystem::GetInstance()._pickableList.push_back(this->GetOwner());
		}
	}
	else
	{
		// 마찬가지로 이미 false인데 다시 false를 입력했을 경우엔 아무것도 안함
		if (!_isPickable)
		{
			return;
		}
		// true였다가 false가 되었다면 컨테이너에서 제거함
		else
		{
			this->_mesh->SetPickable(param);
			
			auto iter = find(GraphicsSystem::GetInstance()._pickableList.begin(), GraphicsSystem::GetInstance()._pickableList.end(), this->GetOwner());
			if (iter != GraphicsSystem::GetInstance()._pickableList.end())
			{
				GraphicsSystem::GetInstance()._pickableList.erase(iter);
			}
		}
	}

	_isPickable = param;
}

bool KunrealEngine::MeshRenderer::GetPickableState()
{
	return this->_isPickable;
}

void KunrealEngine::MeshRenderer::SetRenderingState(bool flag)
{
	if (this->_mesh != nullptr)
	{
		this->_mesh->SetRenderingState(flag);
	}
}

bool KunrealEngine::MeshRenderer::GetRenderingState()
{
	return this->_mesh->GetRenderingState();
}

void KunrealEngine::MeshRenderer::SetDiffuseTexture(int index, const char* textureName)
{
	this->_mesh->SetDiffuseTexture(index, textureName);
	this->_textureName = textureName;
}

std::string KunrealEngine::MeshRenderer::GetTextureName()
{
	return this->_textureName;
}

std::vector<std::string> KunrealEngine::MeshRenderer::GetTextures()
{
	return this->_mesh->GetDiffuseTextureList();
}

void KunrealEngine::MeshRenderer::SetNormalTexture(int index, const char* textureName)
{
	this->_mesh->SetNormalTexture(index, textureName);
	this->_normalName = textureName;
}


void KunrealEngine::MeshRenderer::SetEmissiveTexture(int index, const char* textureName)
{
	this->_mesh->SetEmissiveTexture(index, textureName);
}

std::string KunrealEngine::MeshRenderer::GetNormalName()
{
	if (_mesh != nullptr)
	{
		return this->_normalName;
	}
}

std::vector<std::string> KunrealEngine::MeshRenderer::GetNormals()
{
	return this->_mesh->GetNormalTextureList();
}

void KunrealEngine::MeshRenderer::SetMaterial(GInterface::Material material)
{
	this->_mesh->SetMaterial(material);
}

GInterface::Material KunrealEngine::MeshRenderer::GetMaterial()
{
	if (_mesh != nullptr)
	{
		return this->_mesh->GetMaterial();
	}
}

void KunrealEngine::MeshRenderer::SetReflect(float x, float y, float z, float w)
{
	DirectX::XMFLOAT4 ref(x, y, z, w);

	this->_reflect = ref;
	this->_mesh->SetReflect(ref);
}

DirectX::XMFLOAT4 KunrealEngine::MeshRenderer::GetReflect()
{
	if (_mesh != nullptr)
	{
		return this->_reflect;
	}
}

bool KunrealEngine::MeshRenderer::GetMeshStatus()
{
	if (_mesh == nullptr)
	{
		return false;
	}
	else
	{
		return true;
	}
}

void KunrealEngine::MeshRenderer::SetParentBone(GameObject* model, const std::string& boneName)
{
	_transform->_haveParentBone = true;
	this->_mesh->SetParentBone(model->GetComponent<MeshRenderer>()->_mesh, boneName);

	this->GetOwner()->SetParent(model);

	this->GetOwner()->GetComponent<Transform>()->SetPosition(0.0f, 0.0f, 0.0f);
	this->GetOwner()->GetComponent<Transform>()->SetRotation(0.0f, 0.0f, 0.0f);
}

void KunrealEngine::MeshRenderer::DeleteParentBone()
{
	_transform->_haveParentBone = false;
	this->_mesh->DeleteParentBone();

	this->GetOwner()->DetachFromParent();
}

void KunrealEngine::MeshRenderer::SetShadowState(bool flag)
{
	this->_mesh->SetShadowState(flag);
}

bool KunrealEngine::MeshRenderer::GetShadowState()
{
	return _mesh->GetShadowState();
}


void KunrealEngine::MeshRenderer::SetCartoonState(bool flag)
{
	this->_mesh->SetCartoonRendering(flag);
}

bool KunrealEngine::MeshRenderer::GetCartoonState()
{
	return this->_mesh->GetCartoonRenderingState();
}


void KunrealEngine::MeshRenderer::SetAlpha(float alpha)
{
	this->_mesh->SetAlpha(alpha);
}


float KunrealEngine::MeshRenderer::GetAlpha()
{
	return this->_mesh->GetAlhpa();
}

DirectX::XMFLOAT4X4 KunrealEngine::MeshRenderer::GetBoneTransform(const std::string& boneName)
{
	return _mesh->GetBoneTransform(boneName);
}

DirectX::XMFLOAT4X4 KunrealEngine::MeshRenderer::GetParentBoneOriginalTransform(const std::string& boneName)
{
	return _mesh->GetParentBoneOriginalTransform(boneName);
}

void KunrealEngine::MeshRenderer::SetDissolve(float value)
{
	this->_mesh->SetDissolve(value);
}

void KunrealEngine::MeshRenderer::SetIsDissolve(bool isDissolve)
{
	this->_mesh->SetIsDissolve(isDissolve);
}

float KunrealEngine::MeshRenderer::GetDissolveValue()
{
	return this->_mesh->GetDissolveValue();
}

bool KunrealEngine::MeshRenderer::GetIsDissolve()
{
	return this->_mesh->GetIsDissolve();
}
