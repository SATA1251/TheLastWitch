#pragma once
#include "Component.h"
#include "GraphicsSystem.h"

/// <summary>
/// ��
/// </summary>

#pragma warning(disable: 4251)

namespace KunrealEngine
{
	class Transform;

	enum LightType
	{
		DirectionalLight,
		PointLight,
		None
	};

	class _DECLSPEC Light : public Component
	{
		friend class GameObject;
	private:
		Light();
	public:
		~Light();

		virtual void Initialize() override;
		virtual void Release() override;

		virtual void FixedUpdate() override;
		virtual void Update() override;
		virtual void LateUpdate() override;

		virtual void OnTriggerEnter() override;
		virtual void OnTriggerStay() override;
		virtual void OnTriggerExit() override;

		// light�� �⺻ state�� false���� �������
		virtual void SetActive(bool active) override;

	private:
		// � light���� ������
		LightType _type;

		// ������Ʈ�� Transform
		Transform* _transform;

	public:
		LightType GetLightType();

	private:
		// light �������� ���� �θ�
		GInterface::GraphicsLight* _light;

	public:
		// light�� �����Ǿ����� Ȯ��
		bool GetLightStatus();

		// light Ȱ��ȭ Ȥ�� ��Ȱ��ȭ
		void SetLightState(bool flag);

	private:
		// Directional Light�� �ʿ��� ������
		DirectX::XMFLOAT4 _ambient;
		DirectX::XMFLOAT4 _diffuse;
		DirectX::XMFLOAT4 _specular;
		DirectX::XMFLOAT3 _direction;

		// ��ġ ����
		DirectX::XMFLOAT3 _offset;

		// �θ� ������Ʈ ���� ����
		DirectX::XMFLOAT3 _decomposedPos;

	private:
		// PointLight�� �ʿ��� ������
		float _rangeP;

	public:
		/// Light ���� �Լ�
		/// Ambient, Diffuse, Specular�� ��� ������ ���� ����
		/// Light �������� ĳ�����ؼ� ���

		void SetAmbient(float x, float y, float z, float w);
		DirectX::XMFLOAT4 GetAmbient();

		void SetDiffuse(float x, float y, float z, float w);
		DirectX::XMFLOAT4 GetDiffuse();

		void SetSpecular(float x, float y, float z, float w);
		DirectX::XMFLOAT4 GetSpecular();

		// ���� ����
		void SetOffSet(float x, float y, float z);
		DirectX::XMFLOAT3 GetOffSet();

	public:
		/// Directional Light ����
		// Directional light ����
		// ����Ʈ �Ű������� ��� ���� 0
		void CreateDirectionalLight(DirectX::XMFLOAT4 ambient = { 0.f, 0.f, 0.f, 0.f }, DirectX::XMFLOAT4 diffuse = { 0.f, 0.f, 0.f, 0.f }, DirectX::XMFLOAT4 specular = { 0.f, 0.f, 0.f, 0.f }, DirectX::XMFLOAT3 direction = { 0.f, 0.f, 0.f });

		void SetDirection(float x, float y, float z);
		DirectX::XMFLOAT3 GetDirection();

	public:
		/// PointLight ����
		// PointLight ����
		// ����Ʈ �Ű������� ��� ���� 0
		// �������� ���� ��ġ�̹Ƿ� transform���� �������� �Ǵϱ� ����
		void CreatePointLight(DirectX::XMFLOAT4 ambient = { 0.f, 0.f, 0.f, 0.f }, DirectX::XMFLOAT4 diffuse = { 0.f, 0.f, 0.f, 0.f }, DirectX::XMFLOAT4 specular = { 0.f, 0.f, 0.f, 0.f }, float range = 0.f, float attenuation = 16.0f);

		void SetPointPosition();

		void SetPointRange(float range);
		float GetPointRange();

		DirectX::XMFLOAT3 GetPointPos();

	public:
		DirectX::XMFLOAT3 ChangeDirectionForPattern(DirectX::XMFLOAT3 pointPos);
		DirectX::XMFLOAT3 ResetDirection(DirectX::XMFLOAT3 currentDirection, float deltaTime, float speed);
	};
}

#pragma warning(default: 4251)