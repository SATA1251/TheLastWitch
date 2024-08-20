/// <summary>
/// Ark Engine
/// ������ �������̽� Ŭ����
/// �پ��� ������ �����ǵ���
/// -> ����� GraphicsInterface���� �����پ��Ƿ� �ʿ� ����
/// 
/// ����ȭ
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