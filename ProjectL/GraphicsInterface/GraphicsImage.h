/// <summary>
/// UI를 위한 이미지 인터페이스
/// 
/// 이건호 윤종화
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

		// 디버그 객체의 트랜스폼 설정	
		virtual void SetTransform(DirectX::XMFLOAT4X4 matrix) abstract;
		// 디버그 객체의 위치값 설정
		virtual void SetPosition(float x = 0.0f, float y = 0.0f) abstract;
		// 디버그 객체의 회전값 설정
		virtual void SetRotation(float angle) abstract;
		// 디버그 객체의 크기값 설정
		virtual void SetScale(float x = 1.0f, float y = 1.0f) abstract;

		virtual void SetAlpha(float alpha) abstract;

		// 디버그 객체 삭제
		virtual void Delete() abstract;

		virtual const DirectX::XMUINT2& GetImageSize() abstract;

		virtual void ChangeImage(const std::string& imageName) abstract;
	};
}