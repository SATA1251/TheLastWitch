#include "ResourceManager.h"
#include "ArkSpriteFont.h"
#include "DXTKFont.h"

ArkEngine::ArkDX11::DXTKFont::DXTKFont(ID3D11Device* p_Device, ID3D11RasterizerState* p_RS, ID3D11DepthStencilState* p_DSS)
	: _device(nullptr), _spriteBatch(nullptr), _rasterizerState(nullptr), _depthStencilState(nullptr)
{
	Initialize(p_Device, p_RS, p_DSS);
}

ArkEngine::ArkDX11::DXTKFont::~DXTKFont()
{
	_depthStencilState = nullptr;
	_rasterizerState = nullptr;
	_device = nullptr;
}

void ArkEngine::ArkDX11::DXTKFont::Initialize(ID3D11Device* p_Device, ID3D11RasterizerState* p_RS, ID3D11DepthStencilState* p_DSS)
{
	_device = p_Device;
	ID3D11DeviceContext* p_DeviceContext = nullptr;
	_device->GetImmediateContext(&p_DeviceContext);

	_spriteBatch = std::make_unique<DirectX::SpriteBatch>(p_DeviceContext);

	CreateFont();

	p_DeviceContext->Release();

	_rasterizerState = p_RS;
	_depthStencilState = p_DSS;
}

void ArkEngine::ArkDX11::DXTKFont::CreateFont()
{
	auto fileNameList = ResourceManager::GetInstance()->FindSpecificResources("Resources/Font", "spritefont");

	for (auto fileName : fileNameList)
	{
		TCHAR* fontName = ConvertToTCHAR(fileName.c_str());
		std::string fontFileName = fileName;

		ArkSpriteFont* tempSpriteFont = new ArkSpriteFont(_device, fontName);
		ResourceManager::GetInstance()->SetResource(fontFileName, tempSpriteFont);
		tempSpriteFont->GetThisFont()->SetLineSpacing(10.0f);

		delete[] fontName;
	}
}

void ArkEngine::ArkDX11::DXTKFont::DrawBasicText(int posX, int posY, const char* text)
{
	auto textList = ResourceManager::GetInstance()->GetTextList();

	TextPosition tempPos;
	tempPos.x = posX;
	tempPos.y = posY;

	auto it = textList.find(tempPos);

	if (it != textList.end())
	{
		if (text != it->second)
		{
			ResourceManager::GetInstance()->SetTextList(posX, posY, text);
		}
	}
	else
	{
		ResourceManager::GetInstance()->SetTextList(posX, posY, text);
	}
}

void ArkEngine::ArkDX11::DXTKFont::DrawColorText(int posX, int posY, float color[4], const char* fileName, const char* text)
{
	TCHAR* tempText = ConvertToTCHAR(text);
	DirectX::XMVECTOR tempColor = DirectX::XMVectorSet(color[0], color[1], color[2], color[3]);

	_spriteBatch->Begin();
	ArkSpriteFont* nowFont = ResourceManager::GetInstance()->GetResource<ArkSpriteFont>("Resources/Font/gulim9k.spritefont");
	nowFont->GetThisFont()->DrawString(_spriteBatch.get(), tempText, DirectX::XMFLOAT2((float)posX, (float)posY), tempColor);
	_spriteBatch->End();

	delete[] tempText;
}

void ArkEngine::ArkDX11::DXTKFont::Draw()
{
	auto textList = ResourceManager::GetInstance()->GetTextList();
	
	DirectX::XMFLOAT4 textColor = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	
	for (const auto& index : textList)
	{
		TCHAR* tempText = ConvertToTCHAR(index.second.c_str());
	
		_spriteBatch->Begin();
		ArkSpriteFont* nowFont = ResourceManager::GetInstance()->GetResource<ArkSpriteFont>("Resources/Font/gulim9k.spritefont");
		nowFont->GetThisFont()->DrawString(_spriteBatch.get(), tempText, DirectX::XMFLOAT2((float)index.first.x, (float)index.first.y), XMLoadFloat4(&textColor));
		_spriteBatch->End();
	
		delete[] tempText;
	}
}

TCHAR* ArkEngine::ArkDX11::DXTKFont::ConvertToTCHAR(const char* str)
{
	int length = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
	TCHAR* tcharString = new TCHAR[length];
	MultiByteToWideChar(CP_UTF8, 0, str, -1, tcharString, length);

	return tcharString;
}