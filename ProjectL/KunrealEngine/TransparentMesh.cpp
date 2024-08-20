#include "TransparentMesh.h"
#include "GameObject.h"
#include "Transform.h"
#include "GraphicsSystem.h"
#include "TimeManager.h"

KunrealEngine::TransparentMesh::TransparentMesh()
	:_transform(nullptr), _tMesh(nullptr)
	, _objectName(), _textureName(), _transparency(1.0f), _isCircle(false), _timer(0.0f), 
	_isRendering(false), _isPlayed(false)
{

}

KunrealEngine::TransparentMesh::~TransparentMesh()
{

}

void KunrealEngine::TransparentMesh::Initialize()
{
	this->_transform = this->GetOwner()->GetComponent<Transform>();
}

void KunrealEngine::TransparentMesh::Release()
{
	if (this->_tMesh != nullptr)
	{
		this->_tMesh->Delete();
	}
}

void KunrealEngine::TransparentMesh::FixedUpdate()
{

}

void KunrealEngine::TransparentMesh::Update()
{
	if (GetMeshStatus())
	{
		// 플레이가 한번 완료되었다면
		if (_isPlayed == true)
		{
			return;
		}

		if (_isRendering)
		{
			this->_tMesh->SetTransform(_transform->GetWorldTM());
			_isPlayed = this->_tMesh->RenderWithTimer(TimeManager::GetInstance().GetDeltaTime(), _timer);
		}
	}
}

void KunrealEngine::TransparentMesh::LateUpdate()
{

}

void KunrealEngine::TransparentMesh::OnTriggerEnter()
{

}

void KunrealEngine::TransparentMesh::OnTriggerStay()
{

}

void KunrealEngine::TransparentMesh::OnTriggerExit()
{

}

void KunrealEngine::TransparentMesh::SetActive(bool active)
{
	this->_isActivated = active;

	if (active)
	{
		this->_tMesh->SetStartFlag(true);
		_isPlayed = false;
	}

	else
	{
		this->_tMesh->Reset();
		_isPlayed = true;
	}
}

void KunrealEngine::TransparentMesh::CreateTMesh(const std::string& objectName, const std::string& textureName, float transparency, bool isCircle)
{
	this->_tMesh = GRAPHICS->CreateTransParentMesh(objectName, textureName, transparency, isCircle);
	this->_tMesh->SetRenderType(0);		// 기본값

	this->_objectName = objectName;
	this->_textureName = textureName;
	this->_transparency = transparency;
	this->_isCircle = isCircle;

	// circle이 아닐때 x의 각도를 90만큼 회전
	//if (!isCircle)
	//{
	//	this->_transform->SetRotation(90.0f, this->_transform->GetRotation().y, this->_transform->GetRotation().z);
	//}
}

bool KunrealEngine::TransparentMesh::GetMeshStatus()
{
	if (_tMesh != nullptr)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void KunrealEngine::TransparentMesh::SetTexture(const std::string& textureName)
{
	if (this->_tMesh != nullptr)
	{
		this->_tMesh->SetTexture(textureName);
		this->_textureName = textureName;
	}
}

void KunrealEngine::TransparentMesh::SetRenderType(unsigned int index)
{
	// 0번 : 기본
	// 1번 : 좌->우로 그림 
	// 2번 : 아래 -> 위로 그림 
	// 3번 중앙 -> 외곽으로 그림
	// 4번 중앙 -> 외곽선은 남긴채로 채워줌 (circle일때만 제대로 동작)
	// 5번 도넛 (전체중에 그 부분만 지워짐)
	// 6번 도넛 (원 중에 내부의 원만큼 지워줌)

	if (this->_tMesh != nullptr)
	{
		this->_tMesh->SetRenderType(index);
	}
}

void KunrealEngine::TransparentMesh::SetTimer(float time)
{
	this->_timer = time;
}

void KunrealEngine::TransparentMesh::Reset()
{
	// tMesh 안 만들고 이걸 불렀다는거 자체가 문제니까 예외처리는 안함
	this->_tMesh->Reset();

	_isPlayed = false;

	_isRendering = false;
}

bool KunrealEngine::TransparentMesh::PlayOnce()
{
	if (this->_isActivated)
	{
		this->_tMesh->SetStartFlag(true);

		// 실행되지 않았다면
		if (!_isPlayed)
		{
			_isRendering = true;
		}

		return _isPlayed;
	}

	else return true;
}


bool KunrealEngine::TransparentMesh::IsPlayed()
{
	return _isPlayed;
}

void KunrealEngine::TransparentMesh::SetExceptRange(const DirectX::XMFLOAT3& center, float range)
{
	this->_tMesh->SetExceptRange(center, range);
}

void KunrealEngine::TransparentMesh::SetDecal(bool tf)
{
	this->_tMesh->SetDecal(tf);
}

void KunrealEngine::TransparentMesh::SetInfinite(bool tf)
{
	this->_tMesh->SetInfinite(tf);
}


void KunrealEngine::TransparentMesh::SetApplyPattern(bool tf)
{
	this->_tMesh->ApplyPattern(tf);
}


bool KunrealEngine::TransparentMesh::GetIsApplyPattern()
{
	return this->_tMesh->GetIsApplyPattern();
}

void KunrealEngine::TransparentMesh::StopPlayingInfinite()
{
	this->_tMesh->SetInfinite(false);

	this->_tMesh->Reset();
}

void KunrealEngine::TransparentMesh::PlayInfinite()
{
	this->_tMesh->SetInfinite(true);
}

void KunrealEngine::TransparentMesh::TransformForceUpdate()
{
	this->_tMesh->SetTransform(_transform->GetWorldTM());
}

bool KunrealEngine::TransparentMesh::CheckRenderFinsh()
{
	if (this->_tMesh->GetRenderTimer() >= this->_timer)
	{
		return true;
	}
	else
	{
		return false;
	}
}
