#pragma once

#include <DirectXMath.h>
#include "CommonHeader.h"
#include "Component.h"

#pragma warning (disable: 4251)

namespace KunrealEngine
{
	class GameObject;

	class _DECLSPEC Transform : public Component
	{
	typedef bool FLAG;

		friend class GameObject;
	private:
		Transform();
	public:
		~Transform();

	public:
		virtual void Initialize() override;
		virtual void Release() override;

		virtual void FixedUpdate() override;
		virtual void Update() override;
		virtual void LateUpdate() override;

		virtual void OnTriggerEnter() override;
		virtual void OnTriggerStay() override;
		virtual void OnTriggerExit() override;

		virtual void SetActive(bool active) override;

	private:
	public:
		DirectX::XMFLOAT3 _position;
		DirectX::XMFLOAT3 _rotation;				// local
		DirectX::XMFLOAT3 _scale;

		DirectX::XMFLOAT4 _quaternion;				// world
		DirectX::XMFLOAT4X4 _worldTM;

	public:
		// ���� ���� ��
		DirectX::XMFLOAT3 _posForBone;
		DirectX::XMFLOAT4 _quatForBone;
		bool _haveParentBone;

		// Rotation�� ����
		void SetRotation(float x, float y, float z);
		void SetRotation(const DirectX::XMFLOAT3& rot);

		// Position�� ����
		void SetPosition(float x, float y, float z);
		void SetPosition(const DirectX::XMFLOAT3& pos);

		// Scale�� ����
		void SetScale(float x, float y, float z);
		void SetScale(const DirectX::XMFLOAT3& scale);
		void SetTotalScale(float scale);

		DirectX::XMFLOAT3 GetPosition();
		DirectX::XMFLOAT3 GetRotation();
		DirectX::XMFLOAT4 GetQuaternion();
		DirectX::XMFLOAT3 GetScale();

	public:
		// WorldTM ���			// SRT
		DirectX::XMFLOAT4X4 CreateWorldTransformMatrix();
		DirectX::XMFLOAT4X4 GetWorldTM();

		// �����Ϳ��� ����� gizmo�� position ���� �Լ�
		void SetPositionWithGizmo(float x, float y, float z);

	private:
		// �θ� ������Ʈ�� ������ �� transform ����
		void RecalculateTransform();

		void RevertToOriginal();

	public:
		float _floatingFactor = 0.0f;
		float _floatingHelper = 0.0f;
	};
}

#pragma warning (default: 4251)