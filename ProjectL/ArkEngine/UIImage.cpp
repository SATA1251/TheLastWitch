#include "ResourceManager.h"
#include "Camera.h"
#include "Transform.h"
#include "CommonStruct.h"
#include "ArkEffect.h"
#include "ArkTexture.h"
#include "d3dx11effect.h"
#include "ArkDevice.h"
#include "ArkBuffer.h"
#include "ArkTexture.h"
#include "UIImage.h"

ArkEngine::ArkDX11::UIImage::UIImage(const std::string& imageName, unsigned int screenWidth, unsigned int screenHeight)
	: _imageName("Resources/Textures/" + imageName), _isRendering(true), _textureSRV(nullptr), _alpha(1.0f)
{
	Initialize();
}

ArkEngine::ArkDX11::UIImage::~UIImage()
{	
	ResourceManager::GetInstance()->DeleteUIImage(this);
}

void ArkEngine::ArkDX11::UIImage::Render(DirectX::DX11::SpriteBatch* sp)
{
	RECT sourceRect = { 0, 0, _originTextureSize.x, _originTextureSize.y };
	int posX = _texturePos.x;
	int posY = _texturePos.y;
	RECT destRect = { _texturePos.x, _texturePos.y, _texturePos.x + _finalTextureSize.x, _texturePos.y + _finalTextureSize.y };
 
	DirectX::XMVECTORF32 color = { 1.0f, 1.0f, 1.0f, _alpha };
	sp->Draw(_textureSRV, destRect, &sourceRect, color, 0.0f);
}

bool ArkEngine::ArkDX11::UIImage::GetRenderingState()
{
	return _isRendering;
}

void ArkEngine::ArkDX11::UIImage::SetRenderingState(bool tf)
{
	_isRendering = tf;
}

const std::string& ArkEngine::ArkDX11::UIImage::GetImageName()
{
	return _imageName;
}

void ArkEngine::ArkDX11::UIImage::SetTransform(DirectX::XMFLOAT4X4 matrix)
{
	
}

void ArkEngine::ArkDX11::UIImage::SetPosition(float x, float y)
{
	_texturePos.x = x;
	_texturePos.y = y;
}

void ArkEngine::ArkDX11::UIImage::SetRotation(float angle)
{
	
}

void ArkEngine::ArkDX11::UIImage::SetScale(float x, float y)
{
	_finalTextureSize.x = _originTextureSize.x * x;
	_finalTextureSize.y = _originTextureSize.y * y;
}

void ArkEngine::ArkDX11::UIImage::SetAlpha(float alpha)
{
	_alpha = alpha;
}

void ArkEngine::ArkDX11::UIImage::Delete()
{
	delete this;
}


const DirectX::XMUINT2& ArkEngine::ArkDX11::UIImage::GetImageSize()
{
	return _finalTextureSize;
}

void ArkEngine::ArkDX11::UIImage::ChangeImage(const std::string& imageName)
{
	_imageName = imageName;

	ChangeTexture();
}

void ArkEngine::ArkDX11::UIImage::Initialize()
{
	SetTexture();

	ResourceManager::GetInstance()->AddUIImage(this);
}

void ArkEngine::ArkDX11::UIImage::SetTexture()
{
	auto texture = ResourceManager::GetInstance()->GetResource<ArkTexture>(_imageName);

	if (texture == nullptr)
	{
		ArkTexture* newTexture = new ArkTexture(_imageName.c_str());
		ResourceManager::GetInstance()->SetTextureNameList(_imageName);

		texture = newTexture;
	}

	_texturePos.x = 0.0f;
	_texturePos.y = 0.0f;
	
	_originTextureSize.x = texture->GetTextureSize().x;
	_originTextureSize.y = texture->GetTextureSize().y;

	_finalTextureSize.x = _originTextureSize.x;
	_finalTextureSize.y = _originTextureSize.y;

	_textureSRV = texture->GetDiffuseMapSRV();
}

void ArkEngine::ArkDX11::UIImage::ChangeTexture()
{
	auto texture = ResourceManager::GetInstance()->GetResource<ArkTexture>(_imageName);

	if (texture == nullptr)
	{
		ArkTexture* newTexture = new ArkTexture(_imageName.c_str());
		ResourceManager::GetInstance()->SetTextureNameList(_imageName);

		texture = newTexture;
	}

	_originTextureSize.x = texture->GetTextureSize().x;
	_originTextureSize.y = texture->GetTextureSize().y;

	_textureSRV = texture->GetDiffuseMapSRV();
}
