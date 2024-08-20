#pragma once
#include "KunrealAPI.h"
#include "Player.h"
#include "Coroutine.h"

namespace DirectX
{
	struct XMFLOAT3;
}

namespace KunrealEngine
{
	class GameObject;

	class _DECLSPEC PlayerMove : public Component
	{
	public:
		PlayerMove();
		~PlayerMove();

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
		Transform* _transform;					// �θ� ������Ʈ�� Transform
		DirectX::XMFLOAT3 _targetPos;			// �̵��� ��ġ
		Player* _playerComp;					// �÷��̾� ������Ʈ
		SoundPlayer* _SoundComp;
		float _errorRange;						// �� ���� �������� �̳��� �����̸� �������� �Ϸ�� ������ �Ǵ�
		int _nodeCount;							// �̵��� ����� ��
		float _movedRange;						// ��÷� �̵��� �Ÿ� üũ�� ����
		float _movableRange;					// ��� ���� �Ÿ�		// ���� ���
		float _posY;							// �ʸ��� �ٸ� ���̸� ������ ����

		int _nodeUpdateCount;					// �׺���̼� while�� Ż��Ұ� ������

		// �̵��� ��ġ�� ���콺 ��Ŭ�� �� ������Ʈ
		void UpdateTargetPosition();

		// �̵� ��ɽ� ��� ������Ʈ
		void UpdateMoveNode();

		// ��� ��ɽ� ��� ������Ʈ
		void UpdateDashNode();

		// ����ϰ� �ִ� �����ΰ�
		bool _isDash;
		bool _isMoving;
		bool _isDashReady;

		// ��� ��ƼŬ
		std::vector<GameObject*> _playerDashParticleStart;
		std::vector<GameObject*> _playerDashParticleEnd;

		// ��� ��ƼŬ �ʱ�ȭ
		void DashParticleSetting();

		// ��Ŭ�� ��ƼŬ �ʱ�ȭ
		void ClickParticleSetting();

		// ��Ŭ�� ��ƼŬ
		std::vector<GameObject*> _clickParticleList;

		// ��Ŭ�� Ÿ�̸�
		float _clickTimer;

		// Ÿ�̸� 
		float _timer;

		// ��ƼŬ ��� ó���� ���� ����
		bool _isDashStart;
		bool _isDashEnd;
		bool _isClick;

		// ���� ������Ʈ
		void UpdateSound();

		// ����� int�� ����
		int _soundwalk;
		int _sounddash;

	public:
		// �÷��̾� �̵�
		void MoveToTarget(DirectX::XMFLOAT3 targetPos, float speed);

		// �÷��̾� ���
		void PlayerDash(DirectX::XMFLOAT3 targetPos, float speed);

		// �÷��̾��� Y ��ǥ�� ������Ű�� �Լ�
		void SetPlayerY(float y);
		float GetPlayerY();

		// Navigation�� �̿��� ������
		void NavigationMove(float speed);
		void NavigationDash(float speed);
		std::vector<std::pair<DirectX::XMFLOAT3, DirectX::XMFLOAT3>> _stopover;

		// ������ �� ���� ������ Ŭ������ �� ����� �������� ��� �����
		void RecalculateNavigation();

		// ������ ��� ����	// �÷��̾ ������ �� ���� ���°� �Ǵ� ��� ��
		void ClearNavigation();

		//����� ī�޶�ȿ���� ���� �Լ�
		bool GetisDashed();
		DirectX::XMFLOAT3 GetTargetPosition();
		float& GetMovedRange();

		// �÷��̾ �����̴� ���� ���ߴ� �Լ�
		void StopPlayer();

		/// ������� ���� �Լ�
		void ShowPlayerInfo();

	private:
		// �ڷ�ƾ
		Coroutine_Func(dashReady)
		{
			auto* dashCooldown = this;
			this->_isDashReady = false;
			Waitforsecond(_playerComp->GetPlayerData()._dashCooldown);
			dashCooldown->_isDashReady = true;
		};
	};
}

