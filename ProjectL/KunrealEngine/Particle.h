#pragma once

#include <DirectXMath.h>
#include "CommonHeader.h"
#include "Component.h"

namespace GInterface
{
	class GraphicsParticle;
}

namespace KunrealEngine
{
	class Transform;

	class _DECLSPEC Particle : public Component
	{
		friend class GameObject;
	public:
		Particle();
		~Particle();

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
		// �׷��Ƚ��� ��ƼŬ ��ü
		GInterface::GraphicsParticle* _particle;

		// ��ƼŬ�� ���� ������Ʈ�� transform
		Transform* _transform;

		DirectX::XMFLOAT2 _size;		// ��ƼŬ ũ��
		float _velocity;				// ���� ����
		bool _random;					// �������� ��Ѹ�����
		float _fadeoutTime;				// ������ ��������ϴ� �ð�
		float _lifeTime;				// �� �ڿ� ������� �Ұǰ�
		DirectX::XMFLOAT3 _color;		// RGB		// �� ���� �߰��ϰڴ�
		DirectX::XMFLOAT3 _direction;	// ���� ����
		DirectX::XMFLOAT3 _rotation;	// ȸ�� ����
		DirectX::XMFLOAT3 _offset;		// ��ġ ����

		DirectX::XMFLOAT3 _decomposedPos;
		DirectX::XMFLOAT3 _decomposedRot;
		DirectX::XMFLOAT3 _decomposedScale;

	public:
		// � ��ƼŬ�� ����� ���ΰ�		// ó���� �ݵ�� �������
		void SetParticleEffect(std::string name, std::string fileName, unsigned int maxParticle);

		// ��ƼŬ ���� Ȥ�� �л� 
		void Start();

		// ��ƼŬ ����/�л縦 �ߴ�
		void Stop();

		// ��ƼŬ�� ó�� ��ġ���� �ٽ� �л�
		void Reset();

		// ��ƼŬ ��ġ
		void SetParticlePos(float x, float y, float z);
		void SetParticlePos(DirectX::XMFLOAT3 pos);

		// ��ƼŬ ũ�� ����
		void SetParticleSize(float x, float y);

		// ��ƼŬ ũ�� ��ȯ
		DirectX::XMFLOAT2 GetParticleSize();

		// ��ƼŬ ���� ���� �� ���Ⱒ���� �������� �Ұ����� ����
		void SetParticleVelocity(float velocity, bool isRandom);

		// ��ƼŬ�� fadeout �Ǵ� �ð� �� ������ ������� �ð� ����	// �� ���� ���� �ʿ� ����
		void SetParticleDuration(float fade, float life);

		// ��ƼŬ�� ���� ������ �Լ�	// �⺻������ ���� ������ ����
		// 0.0 ~ 1.0
		void AddParticleColor(float x, float y, float z);

		// ��ƼŬ ���� ���� ����		// ��ġ�� ���� ���� �ش� �������� ���ӵ��� ����
		/// ���� �Ⱦ��ε� ������ ���߿� ����
		void SetParticleDirection(float x, float y, float z);

		// ��ƼŬ rotation ����
		void SetParticleRotation(float x, float y, float z);

		// Ư�� ������ Ʈ������ ����
		void SetTransform(GameObject* renderable, std::string boneName);

		// offset ����
		void SetOffSet(float x, float y, float z);

		// ��� ���� ���� ���� ����
		void SetParticleAngle(DirectX::XMFLOAT3& angle);
		void SetParticleAngle(float x, float y, float z);

		// ��� ���� ���� ���� ����
		void SetParticleAngle(float angle);
		DirectX::XMFLOAT3& GetParticleAngle();

		void SetParticleCameraApply(bool tf);
		bool GetParticleCameraApply();

	public:
		// ��ƼŬ ���� ���޿�
		DirectX::XMFLOAT2 GetSize();
		float GetVelocity();
		bool GetRandomState();
		float GetFadeOutTime();
		float GetLifeTime();
		DirectX::XMFLOAT3 GetColor();
		DirectX::XMFLOAT3 GetDirection();
		DirectX::XMFLOAT3 GetRotation();

	private:
		GameObject* _parentObject;
		std::string _parentBoneName;
	};
}

