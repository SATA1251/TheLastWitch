/// <summary>
/// 
/// 그래픽스 인터페이스와 PointLight와의
/// 연동을 위한 클래스
/// 
/// 윤종화
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
		// ambient 값 설정
		virtual void SetAmbient(DirectX::XMFLOAT4 ambient) override;
		// diffuse 값 설정
		virtual void SetDiffuse(DirectX::XMFLOAT4 diffuse) override;
		// specular 값 설정
		virtual void SetSpecular(DirectX::XMFLOAT4 specular) override;
		// position 값 설정
		virtual void SetPosition(DirectX::XMFLOAT3 position) override;
		// ragne 값 설정
		virtual void SetRange(float range) override;
		// attenuation 값 설정
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

