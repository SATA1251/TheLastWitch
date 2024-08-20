#pragma once

#include <DirectXMath.h>
#include "CommonHeader.h"
#include "Component.h"
#include "GraphicsSystem.h"

/// <summary>
/// 2D �̹����� ����ϴ� ������Ʈ
/// UI� �̰� ���� ���
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

		float _alpha;		// ����

	public:
		// � �̹����� ����� ������ ����
		void SetImage(std::string imageName);

		void ChangeImage(const std::string& imageName);

		// ��� ���� �̹��� �̸� ��ȯ
		const std::string& GetImageName();

		// �̹��� ��ġ �̵�
		void SetPosition(float x, float y);

		// �̹��� ȸ��	// x, y�θ�
		void SetRotation(float x, float y);

		// �̹��� ũ��	// x, y�θ�
		void SetScale(float x, float y);

		// �̹����� �׸� �� ����
		void SetImageStatus(bool flag);

		// �̹����� ���� ���� ����
		void SetAlpha(float alpha);

		// �̹��� ���İ� ��ȯ
		float GetAlpha();

		// �̹����� �׷����� �ִ� �������� Ȯ��
		bool GetImageStatus();

		// Ŭ���� �ƴ���
		bool IsPicked(int mouseX, int mouseY);

		const DirectX::XMUINT2& GetImageSize();
	};
}

#pragma warning (default: 4251)