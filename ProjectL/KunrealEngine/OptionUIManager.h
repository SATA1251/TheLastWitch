// 옵션UI를 컨트롤 하기 위한 컴포넌트...지만 얘가 겜 시작 할때 만들기도 할거야.
// 역할은 UI의 버튼들과 사운드등 실제 시스템을 연결 해주는거.
// 일단... bgm버튼 배열과 sfx버튼 배열이 필요할것 같다.
#pragma once
#include "CommonHeader.h"
#include "KunrealAPI.h"

namespace KunrealEngine
{
	class OptionUIManager : public Component
	{
	public:
		OptionUIManager();
		~OptionUIManager();

		// Component의 가상함수들 오버라이드
		void Initialize() override;
		void Release() override;

		void FixedUpdate() override;
		void Update() override;
		void LateUpdate() override;

		void OnTriggerEnter() override;
		void OnTriggerStay() override;
		void OnTriggerExit() override;

		void SetActive(bool active) override;

		int DifficultyLevel;
	private:
		void InitializeVolumeButton(std::vector<GameObject*>& vecs, int* steps, DirectX::XMFLOAT2 tlposition);
		void Setvolumebutton(int vol, int* type);
		void setDifficulty();

	private:
		KunrealEngine::Scene* scene;
		KunrealEngine::GameObject* optionuibox;


		// 세팅된 볼륨. 0~10까지 이숫자까지의 버튼은 전부 칠하고 이숫자 이후의 버튼은 회색으로 둔다.
		// 비슷하게 사운드도 이거랑 같이 엮을 예정 -> SetVolume(bgm_step*10)
		// 0까지 사용하므로 기본값은 -1로 둘것.
		int _bgm_step = -1;
		int _sfx_step = -1;

		// 사운드 설정용 버튼들은 여기에 차분하게 모아두기
		std::vector<GameObject*> _bgms;
		std::vector<GameObject*> _sfxs;

		KunrealEngine::GameObject* button_check1;
		KunrealEngine::GameObject* button_check2;
		KunrealEngine::GameObject* button_check3;
	};
}