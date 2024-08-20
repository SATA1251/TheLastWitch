#pragma once
#include <windows.h>
#include "CommonHeader.h"

//
#include <vector>
//

/// <summary>
/// ���ӿ����� �ھ� �κ�
/// ���ӿ��� ������ �ʿ��� �������� �κ��� �ʱ�ȭ �� ����
/// 
/// �̰�ȣ
/// </summary>

/// <summary>
/// �׷��Ƚ� ���� ���漱�� �κ�
/// </summary>

namespace GInterface
{
	class GraphicsInterface;
	class GraphicsImage;
}

namespace DirectX
{
	struct XMFLOAT3;
	struct XMFLOAT2;
}

namespace KunrealEngine
{
	class SceneManager;
	class GameObject;
	class SoundPlayer;

	struct Point3D
	{
		float x, y, z;
	};

	class _DECLSPEC EngineCore
	{
		/// private���� ���߿� �ٲ�
	public:
		EngineCore();
		~EngineCore();

	public:
		void Initialize(HWND hwnd, HINSTANCE hInstance, int screenWidth, int screenHeight);
		void Release();

	private:
		void Update();
		void FixedUpdate();
		void LateUpdate();

	public:
		void UpdateAll();

		// �׷��Ƚ� ���� ������ ���� ������ ����
		void Render();

		void SetEditorMousePos(POINT position);

		float _screenWidth;
		float _screenHeight;
	public:
		// ���� �׽�Ʈ�Ҷ� ����� �� ����� �Լ� ���� �İ� ������ �� ���� ��
		void PlayGround();
		void CheckMousePosition();
		void MakeObstacle();
		void RemoveObstacle();

		void CreateTitleScene();

		// ����Ÿ�̸�
		float _endingTimer = 10.0f;

		// Bezier
	private:
		DirectX::XMFLOAT3 Bezier(DirectX::XMFLOAT3 startPoint, DirectX::XMFLOAT3 p1, DirectX::XMFLOAT3 p2, DirectX::XMFLOAT3 endPoint, float t);
		DirectX::XMFLOAT2 BezierPoint2D(DirectX::XMFLOAT2 startPoint, DirectX::XMFLOAT2 p1, DirectX::XMFLOAT2 p2, DirectX::XMFLOAT2 endPoint, float t);
		std::vector<DirectX::XMFLOAT3> BezierSetting(GameObject* bezierObject);

	private:
		std::vector<GameObject*> _bezierObjectList;
		std::vector<std::vector<DirectX::XMFLOAT3>> _bezierPointsList;
		std::vector<std::vector<DirectX::XMFLOAT3>> _bezierSwordSoulPointsList;
		std::vector<DirectX::XMFLOAT3> _particlePointList;
		std::vector<DirectX::XMFLOAT3> _particleSwordSoulPointList;
		std::vector<DirectX::XMFLOAT2> _particleTelepotyPointList;
		std::vector<float> _timeCountList;
		std::vector<KunrealEngine::GameObject*> _particleTeleportList;

		float _timeCount;
		float _timeCountPlayerR;
		bool _isSettingTimer;
		bool _isBezierStartSetting;
		bool _isBezierTeleportSetting;
		// �������
	private:
		// �׷��Ƚ� ���� ��ü�� �� �κ�	
		GInterface::GraphicsInterface* _gInterface;

		/// �ӽÿ� �̹��� ������Ʈ
		GInterface::GraphicsImage* cursorimage;

		std::vector<GameObject*> _mapParticleList;
		std::vector<GameObject*> _coreBigKamenSword;

		void TitleMapParticle();
		void MoveToMain();



	private:
		// �ΰ��� ���콺 ������
		int _ingameMouseX = 0;
		int _ingameMouseY = 0;

		//�ӽÿ� ������ ���콺 ������
		POINT _editorMousepos = { -1, -1 };

		POINT _finalMousePosition = { 0,0 };

		bool _isEditor;			// ������ ��������

		void FloatingY(GameObject* name, float range);

		void ShiveringLight(GameObject* name);

	private:
		void CreateEndingScene();
		void EndingSceneUpdate();
		SoundPlayer* _endingSoundComp;
		int endingSoundindex = 0;
		GameObject* _endingSoundManager;

		SoundPlayer* _bossSoundComp;
		//GameObject* _endingPlayer;

		GameObject* _endingCamera;
		GameObject* _endingCameraAfter;
		GameObject* _endingLight;

		float CountPlus(float start, float end, float speed);
		float CountPlusLoop(float start, float end, float speed);

		float CountMinus(float start, float end, float speed);

		GameObject* _endingMeteo1;
		GameObject* _endingMeteo2;
		GameObject* _endingMeteo3;
		GameObject* _endingMeteo4;
		GameObject* _endingMeteo5;
		GameObject* _endingMeteo6;
		GameObject* _endingMeteo7;
		GameObject* _endingMeteo8;

		GameObject* _endingObject1;
		GameObject* _endingObject2;
		GameObject* _endingObject3;
		GameObject* _endingObject4;
		GameObject* _endingObject5;
		GameObject* _endingObject6;

		GameObject* _endingEnt;
		GameObject* _endingSpider;
		GameObject* _endingSword;
		GameObject* _endingKachujin;
		GameObject* _endingKunho;

		GameObject* _endingCredit1;
		GameObject* _endingLee;
		GameObject* _endingYoon;
		GameObject* _endingKim;
		GameObject* _endingBae;
		GameObject* _endingJung;
		GameObject* _endingThankYou;
		GameObject* _endingTheme;
		GameObject* _bossTheme;

		float _originalYpos = -20.0f;
		float _endingUIScale = -1.333333f;
	private:
		GameObject* titleCamera;

		float titleUIPosY = 0.0f;
		void SmoothTransition(DirectX::XMFLOAT3 startPos, DirectX::XMFLOAT3 startRot,
			DirectX::XMFLOAT3 endPos, DirectX::XMFLOAT3 endRot, float duration, float deltaTime);
		void BeforeStartTutorial();
	};
}

