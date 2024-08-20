/// <summary>
/// DirectionalLight �������̽�
/// 
/// �̰�ȣ ����ȭ
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
		// ambient �� ����
		virtual void SetAmbient(DirectX::XMFLOAT4 ambient) abstract;
		// diffuse �� ����
		virtual void SetDiffuse(DirectX::XMFLOAT4 diffuse) abstract;
		// specular �� ����
		virtual void SetSpecular(DirectX::XMFLOAT4 specular) abstract;
		// direction �� ����
		virtual void SetDirection(DirectX::XMFLOAT3 direction) abstract;
	};
}