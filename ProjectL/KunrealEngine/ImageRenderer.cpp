#include "ImageRenderer.h"
#include "GameObject.h"
#include "Transform.h"
#include "ToolBox.h"

KunrealEngine::ImageRenderer::ImageRenderer()
	:_image(nullptr), _transform(nullptr), 
	_decomposedPos({ 0.0f, 0.0f, 0.0f }), _decomposedRot({ 0.0f, 0.0f, 0.0f }), _decomposedScale({ 0.0f, 0.0f, 0.0f }), _alpha(1.0f)
{
	
}

KunrealEngine::ImageRenderer::~ImageRenderer()
{

}

void KunrealEngine::ImageRenderer::Initialize()
{
	_transform = GetOwner()->GetComponent<Transform>();
}

void KunrealEngine::ImageRenderer::Release()
{
	if (_image != nullptr)
	{
		_image->Delete();
	}
}

void KunrealEngine::ImageRenderer::FixedUpdate()
{
	
}

void KunrealEngine::ImageRenderer::Update()
{
	// image�� WorldTM�� 0,0�������� �ٽ� ����ϴ� ������ �׷��Ƚ��ʿ� �ִ�
	// �׷��� ������Ʈ�� transform�� ���� worldTM�� �Ѱ��ִ°� �ƴ϶� ���ʿ��� �ٽ� ����ϰ� �ؾ���
	if (_image != nullptr)
	{
		// �θ� ������
		if (this->GetOwner()->GetParent() != nullptr)
		{
			DirectX::XMFLOAT4X4 worldTM = this->GetOwner()->GetComponent<Transform>()->GetWorldTM();
			DirectX::XMMATRIX worldMat = DirectX::XMLoadFloat4x4(&worldTM);

			DirectX::XMVECTOR scale, quaternion, translation;

			DirectX::XMMatrixDecompose(&scale, &quaternion, &translation, worldMat);
			DirectX::XMStoreFloat3(&_decomposedPos, translation);
			_decomposedRot = ToolBox::QuaternionToEulerAngles(quaternion);
			DirectX::XMStoreFloat3(&_decomposedScale, scale);

			_image->SetPosition(_decomposedPos.x, _decomposedPos.y);
			_image->SetRotation(_decomposedRot.z);
			_image->SetScale(_decomposedScale.x, _decomposedScale.y);
		}
		// �θ� ������
		else
		{
			_image->SetPosition(_transform->GetPosition().x, _transform->GetPosition().y);
			_image->SetRotation(_transform->GetRotation().z);
			_image->SetScale(_transform->GetScale().x, _transform->GetScale().y);
		}
	}
}

void KunrealEngine::ImageRenderer::LateUpdate()
{
	
}

void KunrealEngine::ImageRenderer::OnTriggerEnter()
{
	
}

void KunrealEngine::ImageRenderer::OnTriggerStay()
{
	
}

void KunrealEngine::ImageRenderer::OnTriggerExit()
{
	
}

void KunrealEngine::ImageRenderer::SetActive(bool active)
{
	if (_image != nullptr)
	{
		SetImageStatus(active);
	}

	this->_isActivated = active;
}

void KunrealEngine::ImageRenderer::SetImage(std::string imageName)
{
	if (_image != nullptr)
	{
		_image->Delete();
	}

	_image = GRAPHICS->CreateImage(imageName.c_str());
}

void KunrealEngine::ImageRenderer::ChangeImage(const std::string& imageName)
{
	std::string path = "Resources/Textures/";
	_image->ChangeImage(path + imageName);
}               

const std::string& KunrealEngine::ImageRenderer::GetImageName()
{
	if (_image != nullptr)
	{
		return _image->GetImageName();
	}
}

void KunrealEngine::ImageRenderer::SetPosition(float x, float y)
{
	_transform->SetPosition(x, y, 0.f);
}

void KunrealEngine::ImageRenderer::SetRotation(float x, float y)
{
	_transform->SetRotation(x, y, 0.f);
}

void KunrealEngine::ImageRenderer::SetScale(float x, float y)
{
	_transform->SetScale(x, y, 1.f);
}

void KunrealEngine::ImageRenderer::SetImageStatus(bool flag)
{
 	_image->SetRenderingState(flag);
}

void KunrealEngine::ImageRenderer::SetAlpha(float alpha)
{
	_image->SetAlpha(alpha);
	this->_alpha = alpha;
}


float KunrealEngine::ImageRenderer::GetAlpha()
{
	return this->_alpha;
}

bool KunrealEngine::ImageRenderer::GetImageStatus()
{
	if (_image != nullptr)
	{
		return _image->GetRenderingState();
	}
}

bool KunrealEngine::ImageRenderer::IsPicked(int mouseX, int mouseY)
{
	if ((mouseX == -1) || (mouseY == -1))
	{
		return false;
	}
// AABB�� �̿��Ͽ� ���콺 Ŀ���� �� ���� �÷��� �ִ��� Ȯ���Ѵ�.
// top, bottom�� �̹����� ���������� �����Ѵ�.	
	// �̹����� ������ (�»��)
	auto tbpos = GetOwner()->GetComponent<Transform>()->GetPosition();
	// �̹����� ���� (���ϴ�)
	auto tlbpos = GetImageSize();
	// �θ� �ִٸ�? �θ��� �����ǵ� ������� �Ѵ�.
	DirectX::XMFLOAT3 papos = {0.0f,0.0f ,0.0f };
	GameObject* parent = GetOwner()->GetParent();
	if (parent != nullptr)
	{
		papos = GetOwner()->GetParent()->GetComponent<Transform>()->GetPosition();
		
		GameObject* parents = GetOwner()->GetParent()->GetParent();
		while (1)
		{
			if (parents == nullptr)
			{
				break;
			}

			papos.x += parents->GetComponent<Transform>()->GetPosition().x;
			papos.y += parents->GetComponent<Transform>()->GetPosition().y;
			parents = parents->GetParent();

		}
	}


	int posl = tbpos.x + tlbpos.x + papos.x;
	int posb = tbpos.y + tlbpos.y + papos.y;

	bool isaa = false;
	bool isbb = false;

	isaa = (mouseX > tbpos.x + papos.x) && (mouseX < posl);
	isbb = (mouseY > tbpos.y + papos.y) && (mouseY < posb);

	if (isaa && isbb)
	{
		return true;
	}

	return false;
}

const DirectX::XMUINT2& KunrealEngine::ImageRenderer::GetImageSize()
{
	return _image->GetImageSize();
}
