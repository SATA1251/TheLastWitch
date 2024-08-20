#pragma once
#include "CommonHeader.h"

/// <summary>
/// 컴포넌트들의 부모가 될 클래스
/// 컴포넌트를 만들 때 이 클래스를 상속받고 만들면 된다
/// 
/// 이건호
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
		GameObject* _ownerObject;					// 컴포넌트가 어떤 오브젝트에 부여되고 있는지
		bool _isActivated;							// 활성화 여부
		bool _isUnique;								// 중복 추가가 가능한 컴포넌트인지 체크
		std::string _componentName;					// 구분을 위한 이름

	public:
		// 어떤 오브젝트가 컴포넌트를 가지고 있는지
		// 오브젝트에서 AddComponent를 할때 자동으로 실행해줌
		void SetOwner(GameObject* object);			// private로 ownerObject를 가지고 있으니 Setter함수 만들어줌
		
		// 이 컴포넌트의 소유 오브젝트를 반환
		GameObject* GetOwner();

		// 컴포넌트 활성화 여부 반환 true / false
		bool GetActivated();

		// 컴포넌트 이름 반환
		std::string GetComponentName();
	};
}

#pragma warning(default: 4251)