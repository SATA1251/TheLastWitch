#pragma once
#include "CommonHeader.h"
#include "Component.h"
#include "GraphicsSystem.h"

namespace KunrealEngine
{
	class _DECLSPEC Animator : public Component
	{
		friend class GameObject;
		// MeshRenderer 컴포넌트를 통해서만 생성하게 하고 싶은데..
	private:
		Animator(GInterface::GraphicsRenderable* meshRenderable);
	public:
		~Animator();

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
		GInterface::GraphicsRenderable* _mesh;

	public:
		// index의 기준은 resource 저장소에 있는 clip이 기준
		// speed 기본값 10.0, 반복여부 기본값은 false
		bool Play(int index, float speed = 10.0f, bool isLoop = false);

		// index뿐 아니라 클립 이름을 전달해서 실행 가능
		bool Play(std::string clipName, float speed = 10.0f, bool isLoop = false);

		// 현재 실행중인 애니메이션을 스탑		// 프레임을 0으로
		void Stop();

		// 현재 실행중인 애니메이션을 일시정지
		void Pause();

		// 일시정지 된 애니메이션을 다시 실행
		void Replay();

		// 현재 실행중인 애니메이션이 몇 프레임인지
		float GetCurrentFrame();

		// 애니메이션을 특정 프레임으로 설정
		void SetCurrentFrame(int frame);

		// 현재 실행중인 애니메이션의 맥스 프레임 가져오기
		float GetMaxFrame();

		// mesh에 어떤 애니메이션 클립들이 들어있는지 확인
		const std::vector<std::string>& GetClipNames();

		// 현재 실행중인 애니메이션 이름 가져오기
		const std::string& GetNowAnimationName();

		// 애니메이션 실행중인지 여부 가져오기
		bool GetIsAnimationPlaying();
	};
}

