/// <summary>
/// iD3DX11Effect를
/// IResource로 상속받기 위해 래핑한 클래스
/// 
/// 윤종화
/// </summary>

#pragma once
#include "IResource.h"

struct ID3DX11Effect;
struct ID3D11InputLayout;
struct D3D11_INPUT_ELEMENT_DESC;

namespace ArkEngine
{
	namespace ArkDX11 
	{
		class ArkEffect : public ArkEngine::IResource
		{
		public:
			ArkEffect(const char* effectName);
			~ArkEffect();

		public:
			ID3DX11Effect* GetEffect();
			ID3D11InputLayout* GetInputLayOut();

		public:
			virtual void Load(const char* fileName) override;
			virtual void UnLoad(const char* fileName) override;

		private:
			void CreateEffect(const char* effectName);
			D3D11_INPUT_ELEMENT_DESC* GetInputDesc(const char* effectName);

		private:
			ID3DX11Effect* _effect;
			ID3D11InputLayout* _inputLayOut;

		private:
			unsigned int _vertexDescIndex;
		};
	}
}

