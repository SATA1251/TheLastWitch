/// <summary>
/// 
/// 카메라를 생성할 때 필수 요소를 담은 인터페이스
/// 
/// 2023.09.26 YJH
/// </summary>
 
#pragma once
namespace ArkEngine
{
	class ICamera abstract
	{
	public:
		ICamera() {};
		virtual ~ICamera() {};

	public:
		virtual void Update() abstract;

	public:
		virtual bool GetMain() abstract;
		virtual void SetMain(bool tf) abstract;

	public:
		virtual void Delete() abstract;

	public:
		virtual void SetProjectionMatrix(float fovY, float aspect, float nz, float fz, bool isPerspective) abstract;

		virtual void Strafe(float deltaTime) abstract;
		virtual void Walk(float deltaTime) abstract;
		virtual void UpDown(float deltaTime) abstract;

	public:
		virtual void SetCameraNewPosition(const DirectX::XMFLOAT3& cameraPos, const DirectX::XMFLOAT3& targetPos) abstract;
		virtual void SetCameraPos(const DirectX::XMFLOAT3& targetPos, const DirectX::XMFLOAT3& direction, float scaleFactor) abstract;

	public:
		// View 행렬 가져오기
		virtual const DirectX::XMMATRIX GetViewMatrix() abstract;

		// 투영 행렬 가져오기
		virtual const DirectX::XMMATRIX GetProjMatrix() abstract;

		// 카메라의 위치 가져오기
		virtual const DirectX::XMFLOAT3 GetCameraPos() abstract;

	public:
		virtual std::vector<DirectX::XMFLOAT4>& GetFrustum() abstract;
	};
}