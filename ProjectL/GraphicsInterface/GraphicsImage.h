/// <summary>
/// UI�� ���� �̹��� �������̽�
/// 
/// �̰�ȣ ����ȭ
/// </summary>

#pragma once

namespace DirectX
{
	struct XMFLOAT4X4;
	struct XMUINT2;
}

namespace GInterface
{
	class GraphicsImage
	{
	public:
		virtual ~GraphicsImage() {};

	public:
	public:
		virtual bool GetRenderingState() abstract;
		virtual void SetRenderingState(bool tf) abstract;

	public:
		virtual const std::string& GetImageName() abstract;

		// ����� ��ü�� Ʈ������ ����	
		virtual void SetTransform(DirectX::XMFLOAT4X4 matrix) abstract;
		// ����� ��ü�� ��ġ�� ����
		virtual void SetPosition(float x = 0.0f, float y = 0.0f) abstract;
		// ����� ��ü�� ȸ���� ����
		virtual void SetRotation(float angle) abstract;
		// ����� ��ü�� ũ�Ⱚ ����
		virtual void SetScale(float x = 1.0f, float y = 1.0f) abstract;

		virtual void SetAlpha(float alpha) abstract;

		// ����� ��ü ����
		virtual void Delete() abstract;

		virtual const DirectX::XMUINT2& GetImageSize() abstract;

		virtual void ChangeImage(const std::string& imageName) abstract;
	};
}