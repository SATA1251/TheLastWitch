/// <summary>
/// 
/// UI 이미지를 담기 위한 인터페이스
/// 
/// 윤종화
/// </summary>

#pragma once

namespace DirectX
{
	struct XMFLOAT4X4;
	struct XMUINT2;

	namespace DX11
	{
		class SpriteBatch;
	}
}

namespace ArkEngine
{
	class IUIImage abstract
	{
	public:
		IUIImage() {};
		virtual ~IUIImage() {};

		virtual void Render(DirectX::DX11::SpriteBatch* sp) abstract;

	public:
		virtual bool GetRenderingState() abstract;
		virtual void SetRenderingState(bool tf) abstract;

	public:
		virtual void SetTransform(DirectX::XMFLOAT4X4 matrix) abstract;
		virtual void SetPosition(float x = 0.0f, float y = 0.0f) abstract;
		virtual void SetRotation(float angle) abstract;
		virtual void SetScale(float x = 1.0f, float y = 1.0f) abstract;

		virtual const DirectX::XMUINT2& GetImageSize() abstract;

		virtual void ChangeImage(const std::string& imageName) abstract;
	};
}