#include "Particle.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "GraphicsSystem.h"
#include "ToolBox.h"

KunrealEngine::Particle::Particle()
	:_particle(nullptr), _transform(nullptr),
	_velocity(0.0f), _random(false), _fadeoutTime(0.0f), _lifeTime(0.0f),
	_size({ 0.0f, 0.0f }), _color({ 0.0f, 0.0f, 0.0f }), _direction({ 0.0f, 0.0f, 0.0f }), _rotation({0.0f, 0.0f, 0.0f}),
	_parentObject(nullptr), _parentBoneName(), _offset({ 0.0f, 0.0f, 0.0f }),
	_decomposedPos({ 0.0f, 0.0f, 0.0f }), _decomposedRot({ 0.0f, 0.0f, 0.0f }), _decomposedScale({ 0.0f, 0.0f, 0.0f })
{

}

KunrealEngine::Particle::~Particle()
{

}

void KunrealEngine::Particle::Initialize()
{
	_transform = this->GetOwner()->GetComponent<Transform>();
}

void KunrealEngine::Particle::Release()
{

}

void KunrealEngine::Particle::FixedUpdate()
{

}

void KunrealEngine::Particle::Update()
{
	// 종속 된 bone이 있으면
	if (_transform->_haveParentBone)
	{
		if (_parentObject != nullptr)
		{
			auto mat = GRAPHICS->GetTransform(_parentObject->GetComponent<MeshRenderer>()->GetMeshObject(), _parentBoneName);

			DirectX::XMMATRIX worldTM = DirectX::XMLoadFloat4x4(&mat);

			DirectX::XMVECTOR scale;
			DirectX::XMVECTOR quat;
			DirectX::XMVECTOR translation;

			DirectX::XMMatrixDecompose(&scale, &quat, &translation, worldTM);

			DirectX::XMStoreFloat3(&this->_transform->_posForBone, translation);
			DirectX::XMStoreFloat4(&this->_transform->_quatForBone, quat);
		}
		 
		auto pos = this->GetOwner()->GetComponent<Transform>()->_posForBone;
		SetParticlePos(pos.x + _offset.x, pos.y + _offset.y, pos.z + _offset.z);
	}
	else
	{
		// 부모가 있으면
		if (this->GetOwner()->GetParent() != nullptr)
		{
			DirectX::XMFLOAT4X4 worldTM = this->GetOwner()->GetComponent<Transform>()->GetWorldTM();
			DirectX::XMMATRIX worldMat = DirectX::XMLoadFloat4x4(&worldTM);

			DirectX::XMVECTOR scale, quaternion, translation;

			DirectX::XMMatrixDecompose(&scale, &quaternion, &translation, worldMat);
			DirectX::XMStoreFloat3(&_decomposedPos, translation);
			_decomposedRot = ToolBox::QuaternionToEulerAngles(quaternion);
			DirectX::XMStoreFloat3(&_decomposedScale, scale);

			SetParticlePos(this->_decomposedPos.x + _offset.x, this->_decomposedPos.y + _offset.y, this->_decomposedPos.z + _offset.z);

		}
		// 없으면
		else
		{
			SetParticlePos(this->_transform->GetPosition().x + _offset.x, this->_transform->GetPosition().y + _offset.y, this->_transform->GetPosition().z + _offset.z);
		}
	}
}

void KunrealEngine::Particle::LateUpdate()
{

}

void KunrealEngine::Particle::OnTriggerEnter()
{

}

void KunrealEngine::Particle::OnTriggerStay()
{

}

void KunrealEngine::Particle::OnTriggerExit()
{

}

void KunrealEngine::Particle::SetActive(bool active)
{
	if (active)
	{
		Start();
	}
	else
	{
		Stop();
	}

	this->_isActivated = active;
}

void KunrealEngine::Particle::SetParticleEffect(std::string name, std::string fileName, unsigned int maxParticle)
{
	_particle = GRAPHICS->CreateParticle(name, fileName, maxParticle);
}

void KunrealEngine::Particle::Start()
{
	this->_particle->Start();
}

void KunrealEngine::Particle::Stop()
{
	this->_particle->Stop();
}

void KunrealEngine::Particle::Reset()
{
	this->_particle->Reset();
}

void KunrealEngine::Particle::SetParticlePos(float x, float y, float z)
{
	DirectX::XMFLOAT3 pos = { x, y, z };

	_particle->SetEmitPos(pos);
}


void KunrealEngine::Particle::SetParticlePos(DirectX::XMFLOAT3 pos)
{
	this->_particle->SetEmitPos(pos);
}

void KunrealEngine::Particle::SetParticleSize(float x, float y)
{
	DirectX::XMFLOAT2 size = { x, y };

	_particle->SetParticleSize(size);
	_size = size;
}


DirectX::XMFLOAT2 KunrealEngine::Particle::GetParticleSize()
{
	return this->_size;
}

void KunrealEngine::Particle::SetParticleVelocity(float velocity, bool isRandom)
{
	_particle->SetEmitVelocity(velocity, isRandom);

	this->_velocity = velocity;
	this->_random = isRandom;
}

void KunrealEngine::Particle::SetParticleDuration(float fade, float life)
{
	this->_particle->SetParticleTime(fade, life);

	this->_fadeoutTime = fade;
	this->_lifeTime = life;
}

void KunrealEngine::Particle::AddParticleColor(float x, float y, float z)
{
	DirectX::XMFLOAT3 color = { x, y, z };
	_particle->SetParticleColor(color);

	this->_color = color;
}

void KunrealEngine::Particle::SetParticleDirection(float x, float y, float z)
{
	DirectX::XMFLOAT3 direction = { x, y, z };
	_particle->SetParticleDirection(direction);

	this->_direction = direction;
}


void KunrealEngine::Particle::SetParticleRotation(float x, float y, float z)
{
	DirectX::XMFLOAT3 rotation = { x, y + 180.0f, z };
	_particle->SetParticleRotation(rotation);

	this->_rotation = rotation;
}


void KunrealEngine::Particle::SetTransform(GameObject* renderable, std::string boneName)
{
	if (_transform->_haveParentBone == false)
	{
		_parentObject = renderable;
		_parentBoneName = boneName;

		_transform->_haveParentBone = true;

		if (renderable != this->GetOwner())
		{
			this->GetOwner()->SetParent(renderable);
		}
	}
}


void KunrealEngine::Particle::SetOffSet(float x, float y, float z)
{
	this->_offset.x = x;
	this->_offset.y = y;
	this->_offset.z = z;
}


void KunrealEngine::Particle::SetParticleAngle(DirectX::XMFLOAT3& angle)
{
	_particle->SetParticleAngle(angle);
}


void KunrealEngine::Particle::SetParticleAngle(float x, float y, float z)
{
	DirectX::XMFLOAT3 angle = { x,y,z };
	_particle->SetParticleAngle(angle);
}

DirectX::XMFLOAT3& KunrealEngine::Particle::GetParticleAngle()
{
	return _particle->GetParticleAngle();
}


void KunrealEngine::Particle::SetParticleCameraApply(bool tf)
{
	_particle->SetParticleCameraApply(tf);
}


bool KunrealEngine::Particle::GetParticleCameraApply()
{
	return _particle->GetParticleCameraApply();
}

void KunrealEngine::Particle::SetParticleAngle(float angle)
{
	auto rot = DirectX::XMFLOAT3(0.0f, 0.0f, angle);
	_particle->SetParticleAngle(rot);
}

DirectX::XMFLOAT2 KunrealEngine::Particle::GetSize()
{
	return this->_size;
}

float KunrealEngine::Particle::GetVelocity()
{
	return this->_velocity;
}

bool KunrealEngine::Particle::GetRandomState()
{
	return this->_random;
}

float KunrealEngine::Particle::GetFadeOutTime()
{
	return this->_fadeoutTime;
}

float KunrealEngine::Particle::GetLifeTime()
{
	return this->_lifeTime;
}

DirectX::XMFLOAT3 KunrealEngine::Particle::GetColor()
{
	return this->_color;
}

DirectX::XMFLOAT3 KunrealEngine::Particle::GetDirection()
{
	return this->_direction;
}

DirectX::XMFLOAT3 KunrealEngine::Particle::GetRotation()
{
	return this->_rotation;
}

