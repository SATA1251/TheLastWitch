#pragma once
#include "CommonHeader.h"

/// <summary>
/// ������Ʈ���� �θ� �� Ŭ����
/// ������Ʈ�� ���� �� �� Ŭ������ ��ӹް� ����� �ȴ�
/// 
/// �̰�ȣ
/// </summary>

#pragma warning(disable: 4251)

namespace KunrealEngine
{
	class GameObject;

	class _DECLSPEC Component
	{
		friend class GameObject;
	public:
		Component();
		virtual ~Component();

	private:
		virtual void Initialize() abstract;
		virtual void Release() abstract;

		virtual void FixedUpdate() abstract;
		virtual void Update() abstract;
		virtual void LateUpdate() abstract;

		virtual void OnTriggerEnter() abstract;
		virtual void OnTriggerStay() abstract;
		virtual void OnTriggerExit() abstract;

		virtual void SetActive(bool active) abstract;

	protected:
		GameObject* _ownerObject;					// ������Ʈ�� � ������Ʈ�� �ο��ǰ� �ִ���
		bool _isActivated;							// Ȱ��ȭ ����
		bool _isUnique;								// �ߺ� �߰��� ������ ������Ʈ���� üũ
		std::string _componentName;					// ������ ���� �̸�

	public:
		// � ������Ʈ�� ������Ʈ�� ������ �ִ���
		// ������Ʈ���� AddComponent�� �Ҷ� �ڵ����� ��������
		void SetOwner(GameObject* object);			// private�� ownerObject�� ������ ������ Setter�Լ� �������
		
		// �� ������Ʈ�� ���� ������Ʈ�� ��ȯ
		GameObject* GetOwner();

		// ������Ʈ Ȱ��ȭ ���� ��ȯ true / false
		bool GetActivated();

		// ������Ʈ �̸� ��ȯ
		std::string GetComponentName();
	};
}

#pragma warning(default: 4251)