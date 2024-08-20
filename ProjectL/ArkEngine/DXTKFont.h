/// <summary>
/// DX11���� ��Ʈ�� ����ϱ� ����
/// 
/// 2023.09.13 YJH
/// </summary>

#pragma once
#include <string>
#include <Memory>
#include "DXTK/SpriteFont.h"
#include "DXTK/SpriteBatch.h"

namespace ArkEngine
{
	namespace ArkDX11
	{
		class DXTKFont
		{
		public:
			DXTKFont(ID3D11Device* p_Device, ID3D11RasterizerState* p_RS, ID3D11DepthStencilState* p_DSS);
			~DXTKFont();

		public:
			void Initialize(ID3D11Device* p_Device, ID3D11RasterizerState* p_RS, ID3D11DepthStencilState* p_DSS);

			// Font ���� ���� �����ϴ� ��� Font�� ���� ���ҽ��� ����
			void CreateFont();

			// �̸� ������ ����, ��Ʈ�� �ؽ�Ʈ ���
			void DrawBasicText(int posX, int posY, const char* text);

			// ����ڰ� ����, ��Ʈ �����Ͽ� �ؽ�Ʈ�� ���
			void DrawColorText(int posX, int posY, float color[4], const char* fileName, const char* text);

			void Draw();

		private:
			// font�� ����µ��� TCHAR���� �ʿ��ϱ⿡ ����� ���Ǽ��� ���� ���ο��� ���ڿ��� TCHAR�� ��ȯ�Ͽ� ó��
			TCHAR* ConvertToTCHAR(const char* str);

		private:
			ID3D11Device* _device;
			std::unique_ptr<DirectX::SpriteBatch> _spriteBatch;

			ID3D11RasterizerState* _rasterizerState;
			ID3D11DepthStencilState* _depthStencilState;
		};
	}
}