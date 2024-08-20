#pragma once

#include <DirectXMath.h>
#include "CommonHeader.h"
#include "Component.h"
#include "GraphicsSystem.h"

/// <summary>
/// 2D 이미지를 출력하는 컴포넌트
/// UI등도 이걸 통해 출력
/// </summary>

#pragma warning (disable: 4251)

namespace KunrealEngine
{
	class Transform;

	class _DECLSPEC ImageRenderer : public Component
	{
		friend class GameObject;
	private:
		ImageRenderer();
	public:
		~ImageRenderer();

		void Initialize() override;
		void Release() override;

		void FixedUpdate() override;
		void Update() override;
		void LateUpdate() override;

		void OnTriggerEnter() override;
		void OnTriggerStay() override;
		void OnTriggerExit() override;

		void SetActive(bool active) override;

	private:
		GInterface::GraphicsImage* _image;
		Transform* _transform;

		DirectX::XMFLOAT3 _decomposedPos;
		DirectX::XMFLOAT3 _decomposedRot;
		DirectX::XMFLOAT3 _decomposedScale;

		float _alpha;		// 투명도

	public:
		// 어떤 이미지를 출력할 것인지 설정
		void SetImage(std::string imageName);

		void ChangeImage(const std::string& imageName);

		// 출력 중인 이미지 이름 반환
		const std::string& GetImageName();

		// 이미지 위치 이동
		void SetPosition(float x, float y);

		// 이미지 회전	// x, y로만
		void SetRotation(float x, float y);

		// 이미지 크기	// x, y로만
		void SetScale(float x, float y);

		// 이미지를 그릴 지 여부
		void SetImageStatus(bool flag);

		// 이미지의 알파 값을 조정
		void SetAlpha(float alpha);

		// 이미지 알파값 반환
		float GetAlpha();

		// 이미지가 그려지고 있는 상태인지 확인
		bool GetImageStatus();

		// 클릭이 됐는지
		bool IsPicked(int mouseX, int mouseY);

		const DirectX::XMUINT2& GetImageSize();
	};
}

#pragma warning (default: 4251)