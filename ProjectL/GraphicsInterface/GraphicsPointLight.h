/// <summary>
/// PointLight �������̽�
/// 
/// �̰�ȣ ����ȭ
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
		// ambient �� ����
		virtual void SetAmbient(DirectX::XMFLOAT4 ambient) abstract;
		// diffuse �� ����
		virtual void SetDiffuse(DirectX::XMFLOAT4 diffuse) abstract;
		// specular �� ����
		virtual void SetSpecular(DirectX::XMFLOAT4 specular) abstract;
		// position �� ����
		virtual void SetPosition(DirectX::XMFLOAT3 position) abstract;
		// ragne �� ����
		virtual void SetRange(float range) abstract;
		// attenuation �� ����
		virtual void SetAttenuation(float att) abstract;
	};
}