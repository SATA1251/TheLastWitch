#pragma once
#include <DirectXMath.h>
#include "CommonHeader.h"
#include "Component.h"

/// <summary>
/// ������ ����ϴ� MeshRenderer�� Deffered Rendering���� �׷���
/// -> ������ ������ �� ����
/// 
/// �׷��� ������ ����� ���� ������� ���� componentȭ ��Ŵ
/// </summary>

namespace GInterface
{
	class GraphicsTransparentMesh;
}

#pragma warning (disable: 4251)

namespace KunrealEngine
{
	class Transform;

	class _DECLSPEC TransparentMesh : public Component
	{
		friend class GameObject;
	private:
		TransparentMesh();
	public:
		~TransparentMesh();

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
		Transform* _transform;
		GInterface::GraphicsTransparentMesh* _tMesh;

		std::string _objectName;		// ������Ʈ �̸�
		std::string _textureName;		// �ؽ�ó �̸�
		float _transparency;			// ����
		bool _isCircle;					// �������� ��������
		float _timer;					// �� �ʿ� ���ļ� ��������

	public:			
		// �׸� �� �ִ� ��������
		bool GetMeshStatus();

		// ������Ʈ ���̰� ���� ���� �����������											// ���� 0.0 ~ 1.0
		void CreateTMesh(const std::string& objectName, const std::string& textureName, float transparency = 1.0f, bool isCircle = false);
	
		// ������� TMesh�� �ؽ��� ����
		void SetTexture(const std::string& textureName);

		// � ������� �׸� ���ΰ�	// �󼼴� ���Ǻκ� ����
		void SetRenderType(unsigned int index);

		// �� �ʿ� ���ļ� ������ �Ұ��ΰ�
		void SetTimer(float time);

		// Ÿ�̸� ����
		void Reset();

		bool PlayOnce();

		bool IsPlayed();

		// 5�� ����Ÿ������ �׸��� ������ ������ ����
		void SetExceptRange(const DirectX::XMFLOAT3& center, float range);

		void SetDecal(bool tf);

		void SetInfinite(bool tf);

		void SetApplyPattern(bool tf);

		bool GetIsApplyPattern();

		void StopPlayingInfinite();
		void PlayInfinite();

		// Ʈ������ ���� ������Ʈ
		void TransformForceUpdate();

		bool CheckRenderFinsh();

	private:
		bool _isRendering;
		bool _isPlayed;
	};
}

#pragma warning (default: 4251)