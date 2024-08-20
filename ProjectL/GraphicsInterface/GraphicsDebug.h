/// <summary>
/// �ݶ��̴� ��ġ ǥ�ø� ���� ����� ��ü �������̽� Ŭ����
/// 
/// ����ȭ
/// </summary>

#pragma once

namespace GInterface
{
	class GraphicsDebug
	{
	public:
		virtual ~GraphicsDebug() {};

	public:
		// ����� ��ü �׸��� ���� ��������
		virtual bool GetActive() abstract;
		// ����� ��ü �׸��� ���� ����
		virtual void SetActive(bool tf) abstract;

		// ����� ��ü�� Ʈ������ ����	
		virtual void SetTransform(const DirectX::XMFLOAT4X4& matrix) abstract;
		// ����� ��ü�� ��ġ�� ����
		virtual void SetPosition(float x = 0.0f, float y = 0.0f, float z = 0.0f) abstract;
		// ����� ��ü�� ȸ���� ����
		virtual void SetRotation(float x = 0.0f, float y = 0.0f, float z = 0.0f) abstract;
		// ����� ��ü�� ũ�Ⱚ ����
		virtual void SetScale(float x = 1.0f, float y = 1.0f, float z = 1.0f) abstract;

		// ����� ��ü ����
		virtual void Delete() abstract;
	};
}