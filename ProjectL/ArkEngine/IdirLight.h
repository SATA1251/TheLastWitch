/// <summary>
/// 
/// 그래픽스 인터페이스와 DirectionalLight와의
/// 연동을 위한 클래스
/// 
/// 윤종화
/// </summary>

#pragma once
#include "GraphicsDirLight.h"

namespace ArkEngine
{
	class IdirLight : public GInterface::GraphicsDirLight
	{
	public:
		IdirLight(unsigned int lightIndex);
		~IdirLight();

	public:
		virtual void Delete() override;

	public:
		// ambinet 값 설정
		virtual void SetAmbient(DirectX::XMFLOAT4 ambient) override;
		// diffuse 값 설정
		virtual void SetDiffuse(DirectX::XMFLOAT4 diffuse) override;
		// specular 값 설정
		virtual void SetSpecular(DirectX::XMFLOAT4 specular) override;
		// direction 값 설정
		virtual void SetDirection(DirectX::XMFLOAT3 direction) override;

	public:
		virtual bool GetActive() override;

		virtual void SetActive(bool ox) override;

	public:
		int GetIndex();
		int GetActiveIndex();
		void MinusIndex();
		void MinusActiveIndex();

	private:
		unsigned int _index;
		int _activeIndex;
		bool _isActive;
	};
}

