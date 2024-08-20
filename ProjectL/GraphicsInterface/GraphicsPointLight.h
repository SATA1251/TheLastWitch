/// <summary>
/// PointLight 인터페이스
/// 
/// 이건호 윤종화
/// </summary>

#pragma once
#include "GraphicsLight.h"

namespace GInterface
{
	class GraphicsPointLight : public  GInterface::GraphicsLight
	{
	public:
		virtual ~GraphicsPointLight() {};

	public:
		// ambient 값 설정
		virtual void SetAmbient(DirectX::XMFLOAT4 ambient) abstract;
		// diffuse 값 설정
		virtual void SetDiffuse(DirectX::XMFLOAT4 diffuse) abstract;
		// specular 값 설정
		virtual void SetSpecular(DirectX::XMFLOAT4 specular) abstract;
		// position 값 설정
		virtual void SetPosition(DirectX::XMFLOAT3 position) abstract;
		// ragne 값 설정
		virtual void SetRange(float range) abstract;
		// attenuation 값 설정
		virtual void SetAttenuation(float att) abstract;
	};
}