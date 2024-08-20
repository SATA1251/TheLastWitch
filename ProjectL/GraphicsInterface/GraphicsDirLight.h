/// <summary>
/// DirectionalLight 인터페이스
/// 
/// 이건호 윤종화
/// </summary>

#pragma once
#include "GraphicsLight.h"

namespace GInterface
{
	class GraphicsDirLight : public  GInterface::GraphicsLight
	{
	public:
		virtual ~GraphicsDirLight() {};

	public:
		// ambient 값 설정
		virtual void SetAmbient(DirectX::XMFLOAT4 ambient) abstract;
		// diffuse 값 설정
		virtual void SetDiffuse(DirectX::XMFLOAT4 diffuse) abstract;
		// specular 값 설정
		virtual void SetSpecular(DirectX::XMFLOAT4 specular) abstract;
		// direction 값 설정
		virtual void SetDirection(DirectX::XMFLOAT3 direction) abstract;
	};
}