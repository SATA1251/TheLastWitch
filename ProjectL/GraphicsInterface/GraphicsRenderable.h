/// <summary>
/// Renderable 객체에서 
/// 게임 엔진쪽에서 사용할 함수를 모아놓은 인터페이스 클래스
/// 
/// 윤종화
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
		// 렌더러블 객체 삭제
		virtual void Delete() abstract;
		// 현재 렌더링 여부 가져오기
		virtual bool GetRenderingState() abstract;
		// 렌더링 여부 설정
		virtual void SetRenderingState(bool tf) abstract;

	public:
		// 피킹 가능 여부 설정
		virtual void SetPickable(bool tf) abstract;
		// 피킹 가능 여부 가져오기
		virtual bool GetPickable() abstract;

	public:
		// 렌더러블 객체의 트랜스폼 설정								// WorldTM
		virtual void SetTransform(const DirectX::XMFLOAT4X4& matrix) abstract;
		// 렌더러블 객체의 위치값 설정
		virtual void SetPosition(float x =0.0f, float y=0.0f, float z=0.0f) abstract;
		// 렌더러블 객체의 회전값 설정
		virtual void SetRotation(float x = 0.0f, float y = 0.0f, float z = 0.0f) abstract;
		// 렌더러블 객체의 크기값 설정
		virtual void SetScale(float x = 1.0f, float y = 1.0f, float z = 1.0f) abstract;

		// 렌더러블 객체의 그릴 오브젝트 설정
		virtual void SetModel(const char* fileName) abstract;
		// 렌더러블 객체의 디퓨즈 텍스쳐 설정
		virtual void SetDiffuseTexture(int index, const char* textureName) abstract;
		// 렌더러블 객체의 노말 텍스쳐 설정
		virtual void SetNormalTexture(int index, const char* textureName) abstract;
		// 렌더러블 객체의 emissive 텍스쳐 설정
		virtual void SetEmissiveTexture(int index, const char* textureName) abstract;
		// 렌더러블 객체의 메테리얼 가져오기
		virtual const GInterface::Material GetMaterial() abstract;
		// 렌더러블 객체의 메테리얼 설정
		virtual void SetMaterial(GInterface::Material material) abstract;
		// 렌더러블 객체의 반사값 설정
		virtual void SetReflect(const DirectX::XMFLOAT4& reflect) abstract;

	public:
		virtual const std::vector<std::string> GetDiffuseTextureList() abstract;
		virtual const std::vector<std::string> GetNormalTextureList() abstract;

	public:
		// 렌더러블 객체에 애니메이션 정보가 있다면 실행
		virtual void PlayAnimation(float deltaTime, bool continiousPlay = false) abstract;
		// 실행중인 애니메이션 멈추기
		virtual void StopAnimation() abstract;
		// FBX용 애니메이션 실행 함수 (인덱스로), 실행중인지 여부를 반환
		virtual bool PlayAnimation(float speed, float deltaTime, int animIndex, bool continuousPlay = false) abstract;
		// FBX용 애니메이션 실행 함수 (이름으로), 실행중인지 여부를 반환
		virtual bool PlayAnimation(float speed, float deltaTime, const std::string& animName, bool continuousPlay = false) abstract;
		// Mesh가 가지고 있는 애니메이션 클립 이름들 반환
		virtual const std::vector<std::string>& GetClipNames() abstract;
		// 애니메이션을 현재 프레임에서 멈춘다
		virtual void PauseAnimation() abstract;
		// 멈춘 애니메이션을 다시 재생한다
		virtual void ReplayAnimation() abstract;
		// 현재 프레임을 반환
		virtual float GetCurrentFrame() abstract;
		// 애니메이션을 특정 프레임으로 설정
		virtual void SetCurrentFrame(int frame) abstract;
		// 현재 애니메이션의 최대 프레임을 반환
		virtual float GetMaxFrame() abstract;
		// 현재 애니메이션을 실행중인지 여부 반환
		virtual bool GetIsPlaying() abstract;
		// 현재 실행중인 애니메이션의 이름을 반환
		virtual const std::string& GetNowAnimationName() abstract;
		// 이름으로 특정 본의 transform값 가져오기
		virtual void SetParentBone(GInterface::GraphicsRenderable* model, const std::string& boneName) abstract;
		// 부모 본이 있다면 제거
		virtual void DeleteParentBone() abstract;
		// 부모 본이 존재할 경우 그래픽스 엔진에서 계산한 트랜스폼 값을 사용, 부모가 없는경우 단위행렬 반환 // worldTM
		virtual DirectX::XMFLOAT4X4 GetTransformEffectedByBone() abstract;
		// 부모 본의 입장에서 transform 행렬 넘겨줌
		virtual DirectX::XMFLOAT4X4 GetBoneTransform(const std::string& boneName) abstract;
		// 부모 본의 단독의 transform 행렬 넘겨줌
		virtual DirectX::XMFLOAT4X4 GetParentBoneOriginalTransform(const std::string& boneName) abstract;

	public:
		// 그림자를 가질 것인지 여부 설정 (기본 설정은 true)
		virtual void SetShadowState(bool tf) abstract;
		// 그림자를 가질 가지는지 여부 가져오기
		virtual bool GetShadowState() abstract;

	public:
		// 카툰 렌더링 적용 여부 설정
		virtual void SetCartoonRendering(bool tf) abstract;
		// 카툰 렌더링 적용 여부 가져오기
		virtual bool GetCartoonRenderingState() abstract;

	public:
		// 투명도 설정
		virtual void SetAlpha(float alpha) abstract;
		// 투명도 적용 값 가져오기
		virtual float GetAlhpa() abstract;

	public:
		virtual void SetDissolve(float value) abstract;
		virtual void SetIsDissolve(bool isDissolve) abstract;
		virtual float GetDissolveValue() abstract;
		virtual bool GetIsDissolve()abstract;
	};
}