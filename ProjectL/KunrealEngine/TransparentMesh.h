#pragma once
#include <DirectXMath.h>
#include "CommonHeader.h"
#include "Component.h"

/// <summary>
/// 기존에 사용하던 MeshRenderer는 Deffered Rendering으로 그려줌
/// -> 투명도를 설정할 수 없음
/// 
/// 그래서 투명도를 사용할 개별 기법으로 새로 component화 시킴
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

		std::string _objectName;		// 오브젝트 이름
		std::string _textureName;		// 텍스처 이름
		float _transparency;			// 투명도
		bool _isCircle;					// 원형으로 퍼지는지
		float _timer;					// 몇 초에 걸쳐서 퍼지는지

	public:			
		// 그릴 수 있는 상태인지
		bool GetMeshStatus();

		// 컴포넌트 붙이고 가장 먼저 실행해줘야함											// 투명도 0.0 ~ 1.0
		void CreateTMesh(const std::string& objectName, const std::string& textureName, float transparency = 1.0f, bool isCircle = false);
	
		// 만들어진 TMesh의 텍스쳐 변경
		void SetTexture(const std::string& textureName);

		// 어떤 방법으로 그릴 것인가	// 상세는 정의부분 참조
		void SetRenderType(unsigned int index);

		// 몇 초에 걸쳐서 퍼지게 할것인가
		void SetTimer(float time);

		// 타이머 리셋
		void Reset();

		bool PlayOnce();

		bool IsPlayed();

		// 5번 렌더타입으로 그릴때 도넛의 범위를 지정
		void SetExceptRange(const DirectX::XMFLOAT3& center, float range);

		void SetDecal(bool tf);

		void SetInfinite(bool tf);

		void SetApplyPattern(bool tf);

		bool GetIsApplyPattern();

		void StopPlayingInfinite();
		void PlayInfinite();

		// 트랜스폼 강제 업데이트
		void TransformForceUpdate();

		bool CheckRenderFinsh();

	private:
		bool _isRendering;
		bool _isPlayed;
	};
}

#pragma warning (default: 4251)