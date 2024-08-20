#pragma once
#include <windows.h>
#include "CommonHeader.h"

//
#include <vector>
//

/// <summary>
/// 게임엔진의 코어 부분
/// 게임엔진 구동에 필요한 전반적인 부분을 초기화 및 구동
/// 
/// 이건호
/// </summary>

/// <summary>
/// 그래픽스 엔진 전방선언 부분
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
		/// private으로 나중에 바꿔
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

		// 그래픽스 엔진 사용법에 따라 수정될 예정
		void Render();

		void SetEditorMousePos(POINT position);

		float _screenWidth;
		float _screenHeight;
	public:
		// 로직 테스트할때 여기다 다 만들어 함수 새로 파고 싶으면 또 만들어도 돼
		void PlayGround();
		void CheckMousePosition();
		void MakeObstacle();
		void RemoveObstacle();

		void CreateTitleScene();

		// 엔딩타이머
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
		// 여기까지
	private:
		// 그래픽스 엔진 객체가 들어갈 부분	
		GInterface::GraphicsInterface* _gInterface;

		/// 임시용 이미지 컴포넌트
		GInterface::GraphicsImage* cursorimage;

		std::vector<GameObject*> _mapParticleList;
		std::vector<GameObject*> _coreBigKamenSword;

		void TitleMapParticle();
		void MoveToMain();



	private:
		// 인게임 마우스 포지션
		int _ingameMouseX = 0;
		int _ingameMouseY = 0;

		//임시용 에디터 마우스 포지션
		POINT _editorMousepos = { -1, -1 };

		POINT _finalMousePosition = { 0,0 };

		bool _isEditor;			// 에디터 실행인지

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

