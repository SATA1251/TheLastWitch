/// <summary>
/// 
/// �׷��Ƚ� �������̽��� PointLight����
/// ������ ���� Ŭ����
/// 
/// ����ȭ
/// </summary>

#pragma once
#include "GraphicsPointLight.h"

namespace ArkEngine
{
	namespace ArkDX11
	{
		class DebugObject;
	}
}

namespace ArkEngine
{
	class IPointLight : public GInterface::GraphicsPointLight
	{
	public:
		IPointLight(unsigned int lightIndex);
		~IPointLight();

	public:
		virtual void Delete() override;

	public:
		// ambient �� ����
		virtual void SetAmbient(DirectX::XMFLOAT4 ambient) override;
		// diffuse �� ����
		virtual void SetDiffuse(DirectX::XMFLOAT4 diffuse) override;
		// specular �� ����
		virtual void SetSpecular(DirectX::XMFLOAT4 specular) override;
		// position �� ����
		virtual void SetPosition(DirectX::XMFLOAT3 position) override;
		// ragne �� ����
		virtual void SetRange(float range) override;
		// attenuation �� ����
		virtual void SetAttenuation(float att) override;
	public:
		virtual bool GetActive() override;

		virtual void SetActive(bool ox) override;

	public:
		int GetActiveIndex();
		void MinusIndex();
		void MinusActiveIndex();

	private:
		ArkEngine::ArkDX11::DebugObject* _debugObject;

	private:
		unsigned int _index;
		int _activeIndex;
		bool _isActive;
	};
}

