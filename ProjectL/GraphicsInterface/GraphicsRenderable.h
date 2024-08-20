/// <summary>
/// Renderable ��ü���� 
/// ���� �����ʿ��� ����� �Լ��� ��Ƴ��� �������̽� Ŭ����
/// 
/// ����ȭ
/// </summary>

#pragma once
#include "GraphicsStruct.h"

namespace DirectX
{
	struct XMFLOAT3;
}

namespace GInterface
{
	class GraphicsRenderable
	{
	public:
		virtual ~GraphicsRenderable() {};

	public:
		// �������� ��ü ����
		virtual void Delete() abstract;
		// ���� ������ ���� ��������
		virtual bool GetRenderingState() abstract;
		// ������ ���� ����
		virtual void SetRenderingState(bool tf) abstract;

	public:
		// ��ŷ ���� ���� ����
		virtual void SetPickable(bool tf) abstract;
		// ��ŷ ���� ���� ��������
		virtual bool GetPickable() abstract;

	public:
		// �������� ��ü�� Ʈ������ ����								// WorldTM
		virtual void SetTransform(const DirectX::XMFLOAT4X4& matrix) abstract;
		// �������� ��ü�� ��ġ�� ����
		virtual void SetPosition(float x =0.0f, float y=0.0f, float z=0.0f) abstract;
		// �������� ��ü�� ȸ���� ����
		virtual void SetRotation(float x = 0.0f, float y = 0.0f, float z = 0.0f) abstract;
		// �������� ��ü�� ũ�Ⱚ ����
		virtual void SetScale(float x = 1.0f, float y = 1.0f, float z = 1.0f) abstract;

		// �������� ��ü�� �׸� ������Ʈ ����
		virtual void SetModel(const char* fileName) abstract;
		// �������� ��ü�� ��ǻ�� �ؽ��� ����
		virtual void SetDiffuseTexture(int index, const char* textureName) abstract;
		// �������� ��ü�� �븻 �ؽ��� ����
		virtual void SetNormalTexture(int index, const char* textureName) abstract;
		// �������� ��ü�� emissive �ؽ��� ����
		virtual void SetEmissiveTexture(int index, const char* textureName) abstract;
		// �������� ��ü�� ���׸��� ��������
		virtual const GInterface::Material GetMaterial() abstract;
		// �������� ��ü�� ���׸��� ����
		virtual void SetMaterial(GInterface::Material material) abstract;
		// �������� ��ü�� �ݻ簪 ����
		virtual void SetReflect(const DirectX::XMFLOAT4& reflect) abstract;

	public:
		virtual const std::vector<std::string> GetDiffuseTextureList() abstract;
		virtual const std::vector<std::string> GetNormalTextureList() abstract;

	public:
		// �������� ��ü�� �ִϸ��̼� ������ �ִٸ� ����
		virtual void PlayAnimation(float deltaTime, bool continiousPlay = false) abstract;
		// �������� �ִϸ��̼� ���߱�
		virtual void StopAnimation() abstract;
		// FBX�� �ִϸ��̼� ���� �Լ� (�ε�����), ���������� ���θ� ��ȯ
		virtual bool PlayAnimation(float speed, float deltaTime, int animIndex, bool continuousPlay = false) abstract;
		// FBX�� �ִϸ��̼� ���� �Լ� (�̸�����), ���������� ���θ� ��ȯ
		virtual bool PlayAnimation(float speed, float deltaTime, const std::string& animName, bool continuousPlay = false) abstract;
		// Mesh�� ������ �ִ� �ִϸ��̼� Ŭ�� �̸��� ��ȯ
		virtual const std::vector<std::string>& GetClipNames() abstract;
		// �ִϸ��̼��� ���� �����ӿ��� �����
		virtual void PauseAnimation() abstract;
		// ���� �ִϸ��̼��� �ٽ� ����Ѵ�
		virtual void ReplayAnimation() abstract;
		// ���� �������� ��ȯ
		virtual float GetCurrentFrame() abstract;
		// �ִϸ��̼��� Ư�� ���������� ����
		virtual void SetCurrentFrame(int frame) abstract;
		// ���� �ִϸ��̼��� �ִ� �������� ��ȯ
		virtual float GetMaxFrame() abstract;
		// ���� �ִϸ��̼��� ���������� ���� ��ȯ
		virtual bool GetIsPlaying() abstract;
		// ���� �������� �ִϸ��̼��� �̸��� ��ȯ
		virtual const std::string& GetNowAnimationName() abstract;
		// �̸����� Ư�� ���� transform�� ��������
		virtual void SetParentBone(GInterface::GraphicsRenderable* model, const std::string& boneName) abstract;
		// �θ� ���� �ִٸ� ����
		virtual void DeleteParentBone() abstract;
		// �θ� ���� ������ ��� �׷��Ƚ� �������� ����� Ʈ������ ���� ���, �θ� ���°�� ������� ��ȯ // worldTM
		virtual DirectX::XMFLOAT4X4 GetTransformEffectedByBone() abstract;
		// �θ� ���� ���忡�� transform ��� �Ѱ���
		virtual DirectX::XMFLOAT4X4 GetBoneTransform(const std::string& boneName) abstract;
		// �θ� ���� �ܵ��� transform ��� �Ѱ���
		virtual DirectX::XMFLOAT4X4 GetParentBoneOriginalTransform(const std::string& boneName) abstract;

	public:
		// �׸��ڸ� ���� ������ ���� ���� (�⺻ ������ true)
		virtual void SetShadowState(bool tf) abstract;
		// �׸��ڸ� ���� �������� ���� ��������
		virtual bool GetShadowState() abstract;

	public:
		// ī�� ������ ���� ���� ����
		virtual void SetCartoonRendering(bool tf) abstract;
		// ī�� ������ ���� ���� ��������
		virtual bool GetCartoonRenderingState() abstract;

	public:
		// ���� ����
		virtual void SetAlpha(float alpha) abstract;
		// ���� ���� �� ��������
		virtual float GetAlhpa() abstract;

	public:
		virtual void SetDissolve(float value) abstract;
		virtual void SetIsDissolve(bool isDissolve) abstract;
		virtual float GetDissolveValue() abstract;
		virtual bool GetIsDissolve()abstract;
	};
}