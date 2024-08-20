/// <summary>
/// ��ũ�������� ����� ī�޶�
/// DirectXMath�� ������� ����
/// 
/// ����ȭ
/// </summary>

#pragma once
#include <vector>
#include "ICamera.h"
#include "GraphicsCamera.h"

namespace DirectX
{
	struct XMFLOAT3;
	struct XMFLOAT4;
	struct XMFLOAT4X4;
	struct XMMATRIX;
}

namespace ArkEngine
{
	namespace ArkDX11
	{
		class Transform;

		class Camera : public ICamera, public GInterface::GraphicsCamera
		{
		public:
			Camera(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& target, DirectX::XMFLOAT3& worldUp);
			~Camera();

		public:
			virtual void Update() override;

		public:
			virtual bool GetMain() override;
			virtual void SetMain(bool tf) override;

		public:
			virtual void Delete() override;

		public:
			// ������� ����
			virtual void SetProjectionMatrix(float fieldOfView_Y, float aspect, float nearZ, float farZ, bool isPerspective) override;

			// ī�޶� �¿�� �̵�
			virtual void Strafe(float deltaTime) override;

			// ī�޶� �յڷ� �̵�
			virtual void Walk(float deltaTime) override;

			// ī�޶� ���Ʒ��� �̵�
			virtual void UpDown(float deltaTime) override;

		public:
			virtual std::vector<DirectX::XMFLOAT4>& GetFrustum() override;

		public:
			virtual void SetCameraNewPosition(const DirectX::XMFLOAT3& cameraPos, const DirectX::XMFLOAT3& targetPos) override;
			virtual DirectX::XMFLOAT3 GetCameraPosition() override;
			virtual void SetCameraPosition(const DirectX::XMFLOAT3& position) override;
			virtual void SetTargetPosition(const DirectX::XMFLOAT3& position) override;
			virtual void RotateCamera(const DirectX::XMFLOAT2& angle) override;

		public:
			// Ÿ�� �����ǰ� ī�޶� �ٶ󺸴� ������ ������ ī�޶��� ��ġ�� ���� (Shadow Map)���� ���
			virtual void SetCameraPos(const DirectX::XMFLOAT3& targetPos, const DirectX::XMFLOAT3& direction, float scaleFactor) override;

		public:
			// View ��� ��������
			virtual const DirectX::XMMATRIX GetViewMatrix() override;

			// ���� ��� ��������
			virtual const DirectX::XMMATRIX GetProjMatrix() override;

			// ī�޶��� ��ġ ��������
			virtual const DirectX::XMFLOAT3 GetCameraPos() override;

		public:
			virtual const DirectX::XMFLOAT3 GetRightVector() override;
			virtual const DirectX::XMFLOAT3 GetLookVector() override;
			virtual const DirectX::XMFLOAT3 GetUpVector() override;

		private:
			// pos, target, worldUp�� ���� look, right, up ���͸� ���� �� view ��� ������Ʈ
			void LookAt(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& target, DirectX::XMFLOAT3& worldUp);
			
			// ������ look, right, up ���͸� ����Ͽ� view ��� ������Ʈ
			void UpdateViewMatrix();

			// �������� ������Ʈ
			void UpdateFrustum();
			
		private:
			DirectX::XMFLOAT3 _positionVector;
			DirectX::XMFLOAT3 _targetVector;

			DirectX::XMFLOAT3 _lookVector;
			DirectX::XMFLOAT3 _rightVector;
			DirectX::XMFLOAT3 _upVector;

			DirectX::XMFLOAT4X4 _viewMatrix;
			DirectX::XMFLOAT4X4 _projectionMatrix;

		private:
			bool _isMain;
			std::vector<DirectX::XMFLOAT4> _frustumPlanes;

		private:
			float _cameraSpaceDepth;
			float _depthScalingFactor;
		};
	}
}

