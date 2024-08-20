/// <summary>
/// 
/// d2d�� �̿��� directWrite�� ����Ͽ�
/// ��Ʈ�� ����ϱ� ���� Ŭ����
/// 
/// ����ȭ
/// </summary>

#pragma once
#include <DirectXMath.h>
#include <unordered_map>
#include <dwrite.h>
#include <d2d1_1.h>
#include <d2d1.h>

namespace DirectX
{
	struct XMFLOAT4;
}

struct XMFLOAT4Hash 
{
	std::size_t operator()(const DirectX::XMFLOAT4& v) const 
	{
		std::size_t hash = 0;

		// ����: �� float ���� �ؽÿ� XOR ������ ���
		hash ^= std::hash<float>{}(v.x);
		hash ^= std::hash<float>{}(v.y);
		hash ^= std::hash<float>{}(v.z);
		hash ^= std::hash<float>{}(v.w);

		return hash;
	}
};

// DirectX::XMFLOAT4�� ���� ��� �� �Լ� ����
struct XMFLOAT4Equal 
{
	bool operator()(const DirectX::XMFLOAT4& lhs, const DirectX::XMFLOAT4& rhs) const 
	{
		return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z && lhs.w == rhs.w;
	}
};


struct POINTHash
{
	std::size_t operator()(const POINT& v) const
	{
		std::size_t hash = 0;

		hash ^= std::hash<float>{}(v.x);
		hash ^= std::hash<float>{}(v.y);

		return hash;
	}
};


struct POINTEqual 
{
	bool operator()(const POINT& lhs, const POINT& rhs) const
	{
		return  lhs.x == rhs.x && lhs.y == rhs.y;
	}
};

class ArkTextLayOut
{
public:
	IDWriteTextLayout* layout;
	std::string string;
};

struct ArkFont
{
public:
	ID2D1SolidColorBrush* _brush;
	IDWriteTextFormat* _format;
	IDWriteTextLayout* _layOut;

	std::string text;
	int _posX;
	int _posY;
};

namespace ArkEngine
{
	namespace ArkDX11
	{
		class DWFont
		{
		public:
			DWFont(IDXGISwapChain* swapChain);
			~DWFont();

		public:
			void Initialize();
			void RenderDebug();
			void RenderUI();
			void Finalize();

		public:
			void RenderUIText(int posX, int posY, std::string text, float fontSize = 20.0f, const DirectX::XMFLOAT4& color = DirectX::XMFLOAT4(255.0f, 255.0f, 255.0f, 255.0f));
			void RenderDebugText(int posX, int posY, std::string text, float fontSize = 20.0f, const DirectX::XMFLOAT4& color = DirectX::XMFLOAT4(255.0f,255.0f,255.0f,255.0f));

		private:
			ID2D1SolidColorBrush* CreateBrush(const DirectX::XMFLOAT4& color);

			IDWriteTextFormat* CreateTextFormat(float fontSize);
			IDWriteTextLayout* CreateTextLayout(POINT pos, std::string text, IDWriteTextFormat* textFormat);

		private:
			ID2D1Factory* _d2dFactory;

			IDWriteFactory* _writeFactory;

			IDXGISurface* _surface;
			ID2D1RenderTarget* _d2dRenderTarget;

		private:
			IDXGISwapChain* _swapChain;

			std::unordered_map<float, IDWriteTextFormat*> _textFormat;

			std::unordered_map<POINT, ArkTextLayOut, POINTHash, POINTEqual> _textLayOut;

			std::unordered_map<DirectX::XMFLOAT4, ID2D1SolidColorBrush*, XMFLOAT4Hash, XMFLOAT4Equal> _brushList;

		private:
			std::vector<ArkFont> _debugTextList;
			std::vector<ArkFont> _uiTextList;

		};
	}
}

