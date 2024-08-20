#pragma once

#include <DirectXMath.h>
#include "Component.h"
#include "GraphicsSystem.h"

namespace GInterface
{
	class GraphicsRenderable;
	struct Material;
}

#pragma warning (disable: 4251)

namespace KunrealEngine
{
	class Transform;

	class _DECLSPEC MeshRenderer : public Component
	{
		friend class GameObject;
	private:
		MeshRenderer();
	public:
		~MeshRenderer();

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
		GInterface::GraphicsRenderable* _mesh;
		Transform* _transform;

		// picking ���� ����
		bool _isPickable;

		// �׸��� ǥ�� ����
		bool _isShadow;

		// �����̵� ���� ����
		bool _isCartoon;

	private:
		// �׸��� �׸��� ���� ������
		// �����Ϳ� ��ġ�� ����ֱ� ���� �����͸� ����� ������ ����
		std::string _meshFileName;
		std::string _textureName;
		std::string _normalName;
		DirectX::XMFLOAT4 _reflect;

	public:
		// ó�� ������Ʈ�� ������ �� �����������
		// �Ű������� mesh ���� �̸�, �ؽ�ó �̸� -> �ؽ�ó�� default �Ű������� nullptr ������ ����
		void SetMeshObject(const char* fileName, bool isSolid = true);

		// mesh ������Ʈ �����͸� ��ȯ	// �׷��Ƚ� �ý��ۿ��� �������� ������Ʈ�� ��ȯ�ϴµ� ���
		GInterface::GraphicsRenderable* GetMeshObject();
		std::string GetMeshName();

		// �̹� mesh�� ���� ������ ���� �� ��ü�ϴ� �Լ�
		void ChangeMeshObject(const char* fileName);

		// Picking ���� ���� ����
		void SetPickableState(bool param);

		// Picking ���� ���� ��ȯ
		bool GetPickableState();

	private:
		// �׸��� �׸� �� ���� ����
		void SetRenderingState(bool flag);

	public:
		bool GetRenderingState();

		// �ؽ�ó
		// ó�� ���� �� ���� �־������� ���ص� ��		//��ü�����ε� ��� ����
		void SetDiffuseTexture(int index, const char* textureName);
		std::string GetTextureName();

		// �ؽ�ó �����̳ʸ� ��ȯ
		std::vector<std::string> GetTextures();

		// �븻
		void SetNormalTexture(int index, const char* textureName);

		// �̹̽ú�
		void SetEmissiveTexture(int index, const char* textureName);

		std::string GetNormalName();

		// �븻�� �����̳ʸ� ��ȯ
		std::vector<std::string> GetNormals();

		// ���͸���
		void SetMaterial(GInterface::Material material);
		GInterface::Material GetMaterial();

		// ���÷�Ʈ		// �ݻ���
		void SetReflect(float x, float y, float z, float w);
		DirectX::XMFLOAT4 GetReflect();

		// _mesh�� null�϶�, �� �ƹ��͵� �� ������� �� false �ƴϸ� true
		// �� ������Ʈ�� ó���� SetMeshObject�� �����ؼ� mesh ������ �־�����Ѵ�
		bool GetMeshStatus();

	public:
		void SetParentBone(GameObject* model, const std::string& boneName);
		void DeleteParentBone();

		// �׸��� Ȱ��ȭ ����
		void SetShadowState(bool flag);
		bool GetShadowState();

		// �����̵� Ȱ��ȭ ����
		void SetCartoonState(bool flag);
		bool GetCartoonState();

		void SetAlpha(float alpha);
		float GetAlpha();

		// �θ� ���� �� ���忡 ������ ��ģ Ʈ�������� ��ȯ
		DirectX::XMFLOAT4X4 GetBoneTransform(const std::string& boneName);
		// �θ� �� ��ü�� Ʈ�������� ��ȯ
		DirectX::XMFLOAT4X4 GetParentBoneOriginalTransform(const std::string& boneName);

	public:
		void SetDissolve(float value);
		void SetIsDissolve(bool isDissolve);
		float GetDissolveValue();
		bool GetIsDissolve();

	};
}

#pragma warning (default: 4251)