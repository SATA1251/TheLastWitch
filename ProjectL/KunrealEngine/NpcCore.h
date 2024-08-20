#pragma once
#include "CommonHeader.h"

// 일단 NPC의 특성을 정의 해보자
// NPC는 이동하지 않는다.
// NPC는 플레이어가 가까이 다가갔을 때 상호작용이 활성화 된다.
// 그게 끝이다.
// 고로 NPC는 무겁게 만들기 보단 그야 말로 딱 필요한 것만 들어있게 하면 된다.
// 말걸고 UI 띄우기(석상)
// 말걸면 대사로 설정(스토리) 나오기(마을 사람)
// 이 두 개 만 있으면 되므로 npc는 말걸기, 다음, 대화종료만 있으면 된다.
// 플로우 차트를 짤 필요도 없다.

namespace KunrealEngine
{
	class Transform;

	class _DECLSPEC NpcCore
	{
	public:
	// 생성자
		NpcCore();
		virtual ~NpcCore();
	
	public:
		void Initialize();
		void Release();
	// 나의 정보와 관련된 함수들
		void UpdateNpc();	// NPC가 프레임마다 바뀌어야 하는 부분이 있다면 이부분에서 체크

		void SetNpcName(std::string npcName);
		void SetInteractionRange(int Range);
		void SetInterationStep(int step);

	public:
	// 외부의 정보를 세팅해주기 위한 함수
		void SetPlayer(Transform* playerTransform);	// 플레이어를 받아오는 변수. 플레이어의 위치를 받아와야 함
													// 플레이어 오브젝트나 플레이어 컴포넌트가 아니라 트랜스폼으로 받아오는 이유.
													// 지금은 플레이어의 위치 외 아무것도 필요하지 않기 때문이지

	public:
		std::string _npcName;	// 이 NPC의 이름
		Transform* _transform;	// 이 NPC의 transform

		Transform* _playerTransform; // 플레이어의 트랜스폼

		int _interactionRange;	// 위 bool 타입 변수가 참이 되기 위한 최대한의 거리.

	protected:
		bool _enoughDistance;	// 대화가 가능 할 정도로 가까운 거리라면 참, 멀다면 거짓
		int _interactionStep;	// 상호작용 단계 0을 디폴트로 두고 -1을 예외사항으로 처리
	};
}

