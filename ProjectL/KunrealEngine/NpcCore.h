#pragma once
#include "CommonHeader.h"

// �ϴ� NPC�� Ư���� ���� �غ���
// NPC�� �̵����� �ʴ´�.
// NPC�� �÷��̾ ������ �ٰ����� �� ��ȣ�ۿ��� Ȱ��ȭ �ȴ�.
// �װ� ���̴�.
// ��� NPC�� ���̰� ����� ���� �׾� ���� �� �ʿ��� �͸� ����ְ� �ϸ� �ȴ�.
// ���ɰ� UI ����(����)
// ���ɸ� ���� ����(���丮) ������(���� ���)
// �� �� �� �� ������ �ǹǷ� npc�� ���ɱ�, ����, ��ȭ���Ḹ ������ �ȴ�.
// �÷ο� ��Ʈ�� © �ʿ䵵 ����.

namespace KunrealEngine
{
	class Transform;

	class _DECLSPEC NpcCore
	{
	public:
	// ������
		NpcCore();
		virtual ~NpcCore();
	
	public:
		void Initialize();
		void Release();
	// ���� ������ ���õ� �Լ���
		void UpdateNpc();	// NPC�� �����Ӹ��� �ٲ��� �ϴ� �κ��� �ִٸ� �̺κп��� üũ

		void SetNpcName(std::string npcName);
		void SetInteractionRange(int Range);
		void SetInterationStep(int step);

	public:
	// �ܺ��� ������ �������ֱ� ���� �Լ�
		void SetPlayer(Transform* playerTransform);	// �÷��̾ �޾ƿ��� ����. �÷��̾��� ��ġ�� �޾ƿ;� ��
													// �÷��̾� ������Ʈ�� �÷��̾� ������Ʈ�� �ƴ϶� Ʈ���������� �޾ƿ��� ����.
													// ������ �÷��̾��� ��ġ �� �ƹ��͵� �ʿ����� �ʱ� ��������

	public:
		std::string _npcName;	// �� NPC�� �̸�
		Transform* _transform;	// �� NPC�� transform

		Transform* _playerTransform; // �÷��̾��� Ʈ������

		int _interactionRange;	// �� bool Ÿ�� ������ ���� �Ǳ� ���� �ִ����� �Ÿ�.

	protected:
		bool _enoughDistance;	// ��ȭ�� ���� �� ������ ����� �Ÿ���� ��, �ִٸ� ����
		int _interactionStep;	// ��ȣ�ۿ� �ܰ� 0�� ����Ʈ�� �ΰ� -1�� ���ܻ������� ó��
	};
}

