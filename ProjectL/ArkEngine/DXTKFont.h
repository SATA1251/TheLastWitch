/// <summary>
/// DX11에서 폰트를 사용하기 위함
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

			// Font 폴더 내에 존재하는 모든 Font에 대한 리소스를 생성
			void CreateFont();

			// 미리 정해진 색깔, 폰트로 텍스트 출력
			void DrawBasicText(int posX, int posY, const char* text);

			// 사용자가 색깔, 폰트 지정하여 텍스트를 출력
			void DrawColorText(int posX, int posY, float color[4], const char* fileName, const char* text);

			void Draw();

		private:
			// font를 만드는데는 TCHAR형이 필요하기에 사용자 편의성을 위해 내부에서 문자열을 TCHAR로 변환하여 처리
			TCHAR* ConvertToTCHAR(const char* str);

		private:
			ID3D11Device* _device;
			std::unique_ptr<DirectX::SpriteBatch> _spriteBatch;

			ID3D11RasterizerState* _rasterizerState;
			ID3D11DepthStencilState* _depthStencilState;
		};
	}
}