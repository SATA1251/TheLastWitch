/// <summary>
/// DXTK�� �����ִ� SpriteFont��
/// IResource�� ��ӹޱ� ���� ������ Ŭ����
/// 
/// ����ȭ
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
			// DXTK::SpriteFont�� ������ Ŭ�����̱⿡ ������ �������ֱ� ����
			DirectX::SpriteFont* GetThisFont();

		private:
			DirectX::SpriteFont* _spriteFont;
		};
	}
}

