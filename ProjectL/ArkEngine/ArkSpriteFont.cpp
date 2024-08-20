#include "DXTK/SpriteFont.h"
#include "ResourceManager.h"
#include "ArkSpriteFont.h"

ArkEngine::ArkDX11::ArkSpriteFont::ArkSpriteFont(ID3D11Device* p_device, const wchar_t* fileName)
	: _spriteFont(nullptr)
{
	_spriteFont = new DirectX::SpriteFont(p_device, fileName);
}

ArkEngine::ArkDX11::ArkSpriteFont::~ArkSpriteFont()
{
	delete _spriteFont;
}

void ArkEngine::ArkDX11::ArkSpriteFont::Load(const char* fileName)
{
	ResourceManager::GetInstance()->SetResource(fileName, this);
}

void ArkEngine::ArkDX11::ArkSpriteFont::UnLoad(const char* fileName)
{
	ResourceManager::GetInstance()->DeleteResource(fileName);
}

DirectX::SpriteFont* ArkEngine::ArkDX11::ArkSpriteFont::GetThisFont()
{
	return _spriteFont;
}
