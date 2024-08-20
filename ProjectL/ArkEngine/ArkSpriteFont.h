/// <summary>
/// DXTK에 속해있는 SpriteFont를
/// IResource를 상속받기 위해 래핑한 클래스
/// 
/// 윤종화
/// </summary>

#pragma once
#include "IResource.h"

namespace ArkEngine
{
	namespace ArkDX11
	{
		class ArkSpriteFont : public ArkEngine::IResource
		{
		public:
			ArkSpriteFont(ID3D11Device* p_device, const wchar_t* fileName);
			~ArkSpriteFont();

		public:
			virtual void Load(const char* fileName) override;
			virtual void UnLoad(const char* fileName) override;

		public:
			// DXTK::SpriteFont를 래핑한 클래스이기에 원형을 내보내주기 위함
			DirectX::SpriteFont* GetThisFont();

		private:
			DirectX::SpriteFont* _spriteFont;
		};
	}
}

