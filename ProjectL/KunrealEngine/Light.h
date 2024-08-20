#pragma once
#include "Component.h"
#include "GraphicsSystem.h"

/// <summary>
/// 빛
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

		// light는 기본 state가 false여서 켜줘야함
		virtual void SetActive(bool active) override;

	private:
		// 어떤 light인지 관리용
		LightType _type;

		// 오브젝트의 Transform
		Transform* _transform;

	public:
		LightType GetLightType();

	private:
		// light 종류들의 공통 부모
		GInterface::GraphicsLight* _light;

	public:
		// light가 생성되었는지 확인
		bool GetLightStatus();

		// light 활성화 혹은 비활성화
		void SetLightState(bool flag);

	private:
		// Directional Light에 필요한 정보들
		DirectX::XMFLOAT4 _ambient;
		DirectX::XMFLOAT4 _diffuse;
		DirectX::XMFLOAT4 _specular;
		DirectX::XMFLOAT3 _direction;

		// 위치 여백
		DirectX::XMFLOAT3 _offset;

		// 부모 오브젝트 대응 변수
		DirectX::XMFLOAT3 _decomposedPos;

	private:
		// PointLight에 필요한 정보들
		float _rangeP;

	public:
		/// Light 공통 함수
		/// Ambient, Diffuse, Specular는 모든 종류의 빛에 들어간다
		/// Light 종류별로 캐스팅해서 사용

		void SetAmbient(float x, float y, float z, float w);
		DirectX::XMFLOAT4 GetAmbient();

		void SetDiffuse(float x, float y, float z, float w);
		DirectX::XMFLOAT4 GetDiffuse();

		void SetSpecular(float x, float y, float z, float w);
		DirectX::XMFLOAT4 GetSpecular();

		// 여백 설정
		void SetOffSet(float x, float y, float z);
		DirectX::XMFLOAT3 GetOffSet();

	public:
		/// Directional Light 관련
		// Directional light 생성
		// 디폴트 매개변수로 모든 값은 0
		void CreateDirectionalLight(DirectX::XMFLOAT4 ambient = { 0.f, 0.f, 0.f, 0.f }, DirectX::XMFLOAT4 diffuse = { 0.f, 0.f, 0.f, 0.f }, DirectX::XMFLOAT4 specular = { 0.f, 0.f, 0.f, 0.f }, DirectX::XMFLOAT3 direction = { 0.f, 0.f, 0.f });

		void SetDirection(float x, float y, float z);
		DirectX::XMFLOAT3 GetDirection();

	public:
		/// PointLight 관련
		// PointLight 생성
		// 디폴트 매개변수로 모든 값은 0
		// 포지션은 빛의 위치이므로 transform에서 가져오면 되니까 제외
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