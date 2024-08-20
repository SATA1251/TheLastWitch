/// <summary>
/// Camera ��ü���� 
/// ���� �����ʿ��� ����� �Լ��� ��Ƴ��� �������̽� Ŭ����
/// 
/// ����ȭ
/// </summary>

#pragma once

namespace GInterface
{
	class GraphicsCamera
	{
	public:
		virtual ~GraphicsCamera() {};

	public:
		// ī�޶� ��ü ����
		virtual void Delete() abstract;

	public:
		virtual DirectX::XMFLOAT3 GetCameraPosition() abstract;
		// ī�޶� ��ġ ����
		virtual void SetCameraPosition(const DirectX::XMFLOAT3& position) abstract;
		// ī�޶� �ٶ󺸴� Ÿ�� ��ġ ����
		virtual void SetTargetPosition(const DirectX::XMFLOAT3& position) abstract;
		// ī�޶� ȸ�� angle.x (���Ʒ�), angle.y (�¿�)
		virtual void RotateCamera(const DirectX::XMFLOAT2& angle) abstract;

	public:
		// ī�޶� �ٶ󺸴� ���⿡�� �������� ���ϴ� ���� ��������
		virtual const DirectX::XMFLOAT3 GetRightVector() abstract;
		// ī�޶� �ٶ󺸴� ���� ���� ��������
		virtual const DirectX::XMFLOAT3 GetLookVector() abstract;
		// ī�޶� �ٶ󺸴� ���⿡�� ���� ���ϴ� ���� ��������
		virtual const DirectX::XMFLOAT3 GetUpVector() abstract;

	};
}