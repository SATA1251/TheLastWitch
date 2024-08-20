/// <summary>
/// 
/// 오브젝트를 생성할 때 필수 요소를 담은 인터페이스
/// 
/// 윤종화
/// </summary>

#pragma once

namespace ArkEngine
{
	class ICamera;
	
	namespace ArkDX11
	{
		class FBXAnimator;
		class FBXMesh;
	}
}


namespace ArkEngine
{
	class IRenderable abstract
	{
	public:
		IRenderable() {};
		virtual ~IRenderable() {};

	public:
		virtual void Initialize() abstract;

		virtual void Update(ArkEngine::ICamera* camera) abstract;

		virtual void Render() abstract;

		virtual void Render(int passIndex) abstract;

		virtual void Finalize() abstract;

	public:
		virtual bool GetRenderingState() abstract;
		virtual void SetRenderingState(bool tf) abstract;

	public:
		virtual void SetShadowState(bool tf) abstract;
		virtual bool GetShadowState() abstract;

	public:
		virtual void SetAlpha(float alpha) abstract;
		virtual float GetAlhpa() abstract;

		virtual const float GetDepth(DirectX::XMFLOAT3& cameraPos) abstract;

	public:
		virtual void SetPosition(float x = 0.0f, float y = 0.0f, float z = 0.0f) abstract;
		virtual void SetRotation(float x = 0.0f, float y = 0.0f, float z = 0.0f) abstract;
		virtual void SetScale(float x = 1.0f, float y = 1.0f, float z = 1.0f) abstract;
		virtual void SetDiffuseTexture(int index, const char* textureName) abstract;
		virtual void SetNormalTexture(int index, const char* textureName) abstract;
		virtual void SetEmissiveTexture(int index, const char* textureName) abstract;

		virtual void SetAnimator() abstract;

	public:
		virtual void PlayAnimation(float deltaTime, bool continiousPlay) abstract;
		virtual void StopAnimation() abstract;

	public:
		virtual bool GetPickable() abstract;
		virtual void SetPickable(bool tf) abstract;

		virtual unsigned int GetHashID() abstract;

		virtual void SetMainCamera(ArkEngine::ICamera* camera) abstract;

	public:
		virtual bool GetInsideFrustumState() abstract;

	public:
		virtual const std::string& GetName() abstract;
		virtual const std::string& GetEffectName() abstract;

		virtual bool GetCartoonRenderingState() abstract;

		virtual std::vector<ID3D11ShaderResourceView*>& GetDiffuseSRV() abstract;
		virtual std::vector<ID3D11ShaderResourceView*>& GetNormalSRV() abstract;
		virtual std::vector<ID3D11ShaderResourceView*>& GetEmmisionSRV() abstract;

		virtual ArkEngine::ArkDX11::FBXAnimator* GetAnimator() abstract;
		virtual const DirectX::XMMATRIX GetTransformMat() abstract;
		virtual ArkEngine::ArkDX11::FBXMesh* GetParentMesh() abstract;
		virtual unsigned int GetParentBoneIndex() abstract;
		virtual DirectX::XMMATRIX GetParentBoneTransform() abstract;
		virtual const DirectX::XMFLOAT4 GetColor() abstract;

	public:
		virtual void SetDissolve(float value) abstract;
		virtual void SetIsDissolve(bool isDissolve) abstract;
		virtual float GetDissolveValue() abstract;
		virtual bool GetIsDissolve()abstract;
	};
}