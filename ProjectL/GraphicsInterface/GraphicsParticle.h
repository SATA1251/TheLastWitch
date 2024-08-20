#pragma once
#include <string>

namespace GInterface
{
	class GraphicsParticle
	{
	public:
		GraphicsParticle() {};
		virtual ~GraphicsParticle() {};

	public:
		// ��ƼŬ ����
		virtual void Start() abstract;
		// ��ƼŬ ���� ����
		virtual void Stop() abstract;

		// ��ƼŬ�� ���������� ���� ��ȯ
		virtual bool GetParticleState() abstract;

	public:
		// ��ƼŬ�� ��ġ ����
		virtual void SetEmitPos(const DirectX::XMFLOAT3& emitPos) abstract;
		// ��ƼŬ�� ũ�� ����
		virtual void SetParticleSize(const DirectX::XMFLOAT2& particleSize) abstract;
		// ��ƼŬ�� ���� �ر�� �������� ����
		virtual void SetEmitVelocity(float emitVelocity, bool isRandom) abstract;
		// ��ƼŬ�� ���� �� �������� �ٷ����� �ð�(Fade)�� �����Ҹ�ð�(Life) ����
		virtual void SetParticleTime(float particleFadeTime, float particleLifeTime) abstract;
		// ��ƼŬ�� ���� ����(RGB)
		virtual void SetParticleColor(const DirectX::XMFLOAT3& particleColor) abstract;
		// ��ƼŬ�� ���� ���� ����
		virtual void SetParticleDirection(const DirectX::XMFLOAT3& particleDirection) abstract;
		// ��ƼŬ�� ȸ�� ����
		virtual void SetParticleRotation(const DirectX::XMFLOAT3& particleRotation) abstract;

		// ��ƼŬ ������ ���� �����̼� ���� (��� ���� ����)
		virtual void SetParticleAngle(DirectX::XMFLOAT3& angle) abstract;

		// ��ƼŬ ������ ���� �����̼� ��ȯ
		virtual DirectX::XMFLOAT3& GetParticleAngle() abstract;

		// ��ƼŬ ���� ī�޶� �Ἥ ������ ���� ���� (��� ���� ����)
		virtual void SetParticleCameraApply(bool tf) abstract;

		// ��ƼŬ ���� ī�޶� �Ἥ ������ ���� ���� (��� ���� ����)
		virtual bool GetParticleCameraApply() abstract;

		// ��ƼŬ�� �ٽ� 0���� �����ϰ� �����
		virtual void Reset() abstract;

		// ������ ���������� ���������� ���� ����
		virtual void SetForwardRendering(bool tf) abstract;

		// ������ ���������� ���������� ���� ��������
		virtual bool GetIsForwardRendering() abstract;
	};
}