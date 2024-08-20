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

		// picking 가능 여부
		bool _isPickable;

		// 그림자 표시 여부
		bool _isShadow;

		// 툰쉐이딩 적용 여부
		bool _isCartoon;

	private:
		// 그림을 그리기 위한 정보들
		// 에디터에 수치를 띄워주기 위해 데이터를 멤버로 가지고 있음
		std::string _meshFileName;
		std::string _textureName;
		std::string _normalName;
		DirectX::XMFLOAT4 _reflect;

	public:
		// 처음 컴포넌트를 생성한 후 실행해줘야함
		// 매개변수로 mesh 파일 이름, 텍스처 이름 -> 텍스처는 default 매개변수로 nullptr 가지고 있음
		void SetMeshObject(const char* fileName, bool isSolid = true);

		// mesh 오브젝트 포인터를 반환	// 그래픽스 시스템에서 보유중인 오브젝트를 반환하는데 사용
		GInterface::GraphicsRenderable* GetMeshObject();
		std::string GetMeshName();

		// 이미 mesh를 무언가 가지고 있을 때 교체하는 함수
		void ChangeMeshObject(const char* fileName);

		// Picking 가능 여부 설정
		void SetPickableState(bool param);

		// Picking 가능 여부 반환
		bool GetPickableState();

	private:
		// 그림을 그릴 지 여부 결정
		void SetRenderingState(bool flag);

	public:
		bool GetRenderingState();

		// 텍스처
		// 처음 만들 때 같이 넣어줬으면 안해도 됌		//교체용으로도 사용 가능
		void SetDiffuseTexture(int index, const char* textureName);
		std::string GetTextureName();

		// 텍스처 컨테이너를 반환
		std::vector<std::string> GetTextures();

		// 노말
		void SetNormalTexture(int index, const char* textureName);

		// 이미시브
		void SetEmissiveTexture(int index, const char* textureName);

		std::string GetNormalName();

		// 노말맵 컨테이너를 반환
		std::vector<std::string> GetNormals();

		// 매터리얼
		void SetMaterial(GInterface::Material material);
		GInterface::Material GetMaterial();

		// 리플렉트		// 반사계수
		void SetReflect(float x, float y, float z, float w);
		DirectX::XMFLOAT4 GetReflect();

		// _mesh가 null일때, 즉 아무것도 안 들어있을 때 false 아니면 true
		// 이 컴포넌트는 처음에 SetMeshObject를 실행해서 mesh 파일을 넣어줘야한다
		bool GetMeshStatus();

	public:
		void SetParentBone(GameObject* model, const std::string& boneName);
		void DeleteParentBone();

		// 그림자 활성화 여부
		void SetShadowState(bool flag);
		bool GetShadowState();

		// 툰쉐이딩 활성화 여부
		void SetCartoonState(bool flag);
		bool GetCartoonState();

		void SetAlpha(float alpha);
		float GetAlpha();

		// 부모 본과 내 월드에 영향을 합친 트랜스폼을 반환
		DirectX::XMFLOAT4X4 GetBoneTransform(const std::string& boneName);
		// 부모 본 자체의 트랜스폼을 반환
		DirectX::XMFLOAT4X4 GetParentBoneOriginalTransform(const std::string& boneName);

	public:
		void SetDissolve(float value);
		void SetIsDissolve(bool isDissolve);
		float GetDissolveValue();
		bool GetIsDissolve();

	};
}

#pragma warning (default: 4251)