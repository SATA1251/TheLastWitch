/// <summary>
/// Ark Engine
/// 렌더러 인터페이스 클래스
/// 다양한 엔진에 대응되도록
/// -> 현재는 GraphicsInterface에서 가져다쓰므로 필요 없음
/// 
/// 윤종화
/// </summary>

#pragma once
#include <windows.h>

namespace ArkEngine
{
	class IRenderable;
}

namespace ArkEngine
{
	class IRenderer abstract
	{
	public:
		IRenderer() {};
		virtual ~IRenderer() {};

	public:
		virtual void Initialize(long long hwnd, int clientWidth, int clientHeight) abstract;

		virtual void Initialize(long long hwnd, int clientWidth, int clientHeight, float backGroundColor[4]) abstract;

		virtual IRenderable* CreateRenderable(const char* fileName, const char* textureName = nullptr) abstract;

		virtual void DeleteRenderable(IRenderable* renderable) abstract;

		virtual void Update() abstract;

		virtual void BeginRender() abstract;

		virtual void Render() abstract;

		virtual void EndRender() abstract;

		virtual void Finalize() abstract;

	public:
		virtual void DrawBasicText(int posX, int posY, const char* text, ...) abstract;
		virtual void DrawColorText(int posX, int posY, float color[4], const char* fontName, const char* text, ...) abstract;
	};
}

extern "C" __declspec(dllexport) ArkEngine::IRenderer * CreateGraphics();

extern "C" __declspec(dllexport) void ReleaseGraphics(ArkEngine::IRenderer * instance);