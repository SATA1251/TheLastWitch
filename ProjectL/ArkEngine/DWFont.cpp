#include <iostream>
#include <cstring>
#include <cwchar>
#include "ResourceManager.h"
#include "ArkDevice.h"
#include "DWFont.h"

ArkEngine::ArkDX11::DWFont::DWFont(IDXGISwapChain* swapChain)
	: _d2dFactory(nullptr), _writeFactory(nullptr),
	_swapChain(swapChain), _surface(nullptr), _d2dRenderTarget(nullptr),
	_textFormat(), _textLayOut(), _brushList(), _debugTextList()
{
	Initialize();
}

ArkEngine::ArkDX11::DWFont::~DWFont()
{
	Finalize();
}

void ArkEngine::ArkDX11::DWFont::Initialize()
{
	auto options = D2D1_FACTORY_OPTIONS();
	options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
	HRESULT result = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, options, &_d2dFactory);
	
	result = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&_writeFactory));
	
	result = _swapChain->GetBuffer(0, IID_PPV_ARGS(&_surface));
	
	auto d2dRTProps = D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_DEFAULT, D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED), 0, 0);
	
	result = _d2dFactory->CreateDxgiSurfaceRenderTarget(_surface, &d2dRTProps, &_d2dRenderTarget);
}

void ArkEngine::ArkDX11::DWFont::RenderDebug()
{
	_d2dRenderTarget->BeginDraw();

	for (const auto& index : _debugTextList)
	{
		_d2dRenderTarget->DrawTextLayout(D2D1::Point2F(index._posX, index._posY), index._layOut, index._brush);
	}
	
	_d2dRenderTarget->EndDraw();
}


void ArkEngine::ArkDX11::DWFont::RenderUI()
{
	_d2dRenderTarget->BeginDraw();

	for (const auto& index : _uiTextList)
	{
		_d2dRenderTarget->DrawTextLayout(D2D1::Point2F(index._posX, index._posY), index._layOut, index._brush);
	}

	_d2dRenderTarget->EndDraw();
}

void ArkEngine::ArkDX11::DWFont::Finalize()
{
	for (auto&[key, value]  : _brushList)
	{
		ID2D1SolidColorBrush* brush = value;
		if (brush != nullptr)
		{
			brush->Release();
		}
	}
	_brushList.clear();
	
	for (auto& [key, value] : _textLayOut)
	{
		IDWriteTextFormat* textFormat = value.layout;
		if (textFormat != nullptr)
		{
			textFormat->Release();
		}
	}
	
	for (auto&[key, value] : _textFormat)
	{
		IDWriteTextFormat* textFormat = value;
		if (textFormat != nullptr)
		{
			textFormat->Release();
		}
	}
	_textFormat.clear();

	_d2dRenderTarget->Release();
	_surface->Release();

	_writeFactory->Release();

	_d2dFactory->Release();
}


void ArkEngine::ArkDX11::DWFont::RenderUIText(int posX, int posY, std::string text, float fontSize /*= 20.0f*/, const DirectX::XMFLOAT4& color /*= DirectX::XMFLOAT4(255.0f, 255.0f, 255.0f, 255.0f)*/)
{
	for (auto index = 0; index < _uiTextList.size(); index++)
	{
		if (_uiTextList[index]._posX == posX && _uiTextList[index]._posY == posY)
		{
			if (_uiTextList[index].text == text)
			{
				return;
			}

			_uiTextList.erase(_uiTextList.begin() + index);

			break;
		}
	}

	POINT pos = { posX, posY };

	auto brush = CreateBrush(color);
	auto textFormat = CreateTextFormat(fontSize);
	auto textLayOut = CreateTextLayout(pos, text, textFormat);

	ArkFont newFont;
	newFont._brush = brush;
	newFont._format = textFormat;
	newFont._layOut = textLayOut;
	newFont.text = text;
	newFont._posX = posX;
	newFont._posY = posY;

	_uiTextList.emplace_back(newFont);
}

void ArkEngine::ArkDX11::DWFont::RenderDebugText(int posX, int posY, std::string text, float fontSize, const DirectX::XMFLOAT4& color)
{
	for (auto index = 0; index < _debugTextList.size(); index++)
	{
		if (_debugTextList[index]._posX == posX && _debugTextList[index]._posY == posY)
		{
			if (_debugTextList[index].text == text)
			{
				return;
			}

			_debugTextList.erase(_debugTextList.begin() + index);

			break;
		}
	}

	POINT pos = { posX, posY };

	auto brush = CreateBrush(color);
	auto textFormat = CreateTextFormat(fontSize);
	auto textLayOut = CreateTextLayout(pos, text, textFormat);

	ArkFont newFont;
	newFont._brush = brush;
	newFont._format = textFormat;
	newFont._layOut = textLayOut;
	newFont.text = text;
	newFont._posX = posX;
	newFont._posY = posY;

	_debugTextList.emplace_back(newFont);
}

ID2D1SolidColorBrush* ArkEngine::ArkDX11::DWFont::CreateBrush(const DirectX::XMFLOAT4& color)
{
	auto iter = _brushList.find(color);

	if (iter == _brushList.end())
	{
		ID2D1SolidColorBrush* newBrush = nullptr;
		_brushList.insert(std::make_pair(color, newBrush));

		_d2dRenderTarget->CreateSolidColorBrush(D2D1::ColorF(color.x/255.0f, color.y/ 255.0f, color.z/ 255.0f, color.w/ 255.0f), &_brushList.at(color));
	}

	return _brushList.at(color);

}

IDWriteTextFormat* ArkEngine::ArkDX11::DWFont::CreateTextFormat(float fontSize)
{
	auto iter = _textFormat.find(fontSize);

	if (iter == _textFormat.end())
	{
		IDWriteTextFormat* newFormat = nullptr;
		_textFormat.insert(std::make_pair(fontSize, newFormat));

		_writeFactory->CreateTextFormat(
			L"default", nullptr, DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_NORMAL,
			DWRITE_FONT_STYLE::DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH::DWRITE_FONT_STRETCH_NORMAL,
			fontSize, L"ko", &_textFormat.at(fontSize));
	}

	return _textFormat.at(fontSize);
}


IDWriteTextLayout* ArkEngine::ArkDX11::DWFont::CreateTextLayout(POINT pos, std::string text, IDWriteTextFormat* textFormat)
{
	auto iter = _textLayOut.find(pos);

	if (iter == _textLayOut.end())
	{
		ArkTextLayOut newLayOut;
		newLayOut.string = text;

		std::size_t length = text.length() + 1; // +1은 null 문자('\0')를 위함
		std::wstring wideString(length, L'\0');
		size_t convertedChars = 0;
		mbstowcs_s(&convertedChars, &wideString[0], length, text.c_str(), _TRUNCATE);

		_writeFactory->CreateTextLayout(wideString.c_str(), wcslen(wideString.c_str()), textFormat, 800, 500, &newLayOut.layout);

		_textLayOut[pos] = newLayOut;
	}
	else 
	{
		if (_textLayOut.at(pos).string != text)
		{
			// 기존 레이아웃 해제
			_textLayOut[pos].layout->Release();
			_textLayOut[pos].layout = nullptr;

			ArkTextLayOut newLayOut;
			newLayOut.string = text;

			std::size_t length = text.length() + 1; // +1은 null 문자('\0')를 위함
			std::wstring wideString(length, L'\0');
			size_t convertedChars = 0;
			mbstowcs_s(&convertedChars, &wideString[0], length, text.c_str(), _TRUNCATE);

			_writeFactory->CreateTextLayout(wideString.c_str(), wcslen(wideString.c_str()), textFormat, 800, 500, &newLayOut.layout);

			_textLayOut[pos] = newLayOut;

		}
	}

	return _textLayOut.at(pos).layout;
}