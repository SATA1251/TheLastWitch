/// <summary>
/// 
/// 렌더링할 FBX 메쉬
/// 
/// 김현재
/// </summary>

#pragma once
#include <string>
#include <array>
// Buffer사용하기위해
#include "d3d11.h"
#include "CommonStruct.h"
#include "GraphicsRenderable.h"
#include "IRenderable.h"

#include "tinyxml2.h"
#include "ParsingStructs.h"

namespace DirectX
{
	struct XMFLOAT4;
	struct XMFLOAT4X4;
}

namespace ArkEngine
{
	class IDebugObject;

	namespace ArkDX11
	{
		class Transform;
		class Animator;

		struct Material;

		class FBXAnimator;

		class ArkEffect;
		class ArkDevice;

		//class DirectionalLight;
	}
}

/// 김현재 추가
struct ModelMesh;
struct ID3DX11EffectScalarVariable;

namespace ArkEngine
{
	namespace ArkDX11
	{
		class FBXMesh : public ArkEngine::IRenderable, public GInterface::GraphicsRenderable
		{
		public:
			FBXMesh(const std::string& fileName, bool isSolid = true);
			~FBXMesh();

		public:
			virtual void Initialize() override;
			virtual void Update(ArkEngine::ICamera* p_Camera) override;
			virtual void Render() override;
			virtual void Render(int passIndex) override;
			virtual void Finalize() override;

		public:
			virtual void Delete() override;

		public:
			virtual bool GetRenderingState() override;
			virtual void SetRenderingState(bool tf) override;

		public:
			virtual void SetShadowState(bool tf) override;
			virtual bool GetShadowState() override;

		public:
			virtual void SetTransform(const DirectX::XMFLOAT4X4& matrix) override;
			virtual void SetPosition(float x = 0.0f, float y = 0.0f, float z = 0.0f) override;
			virtual void SetRotation(float x = 0.0f, float y = 0.0f, float z = 0.0f) override;
			virtual void SetScale(float x = 1.0f, float y = 1.0f, float z = 1.0f) override;
			virtual void SetModel(const char* fileName) override;
			virtual void SetDiffuseTexture(int index, const char* textureName) override;
			virtual void SetNormalTexture(int index, const char* textureName) override;
			virtual void SetEmissiveTexture(int index, const char* textureName) override;
			virtual void SetAnimator() override;
			virtual const GInterface::Material GetMaterial() override;
			virtual void SetMaterial(GInterface::Material material) override;
			virtual void SetReflect(const DirectX::XMFLOAT4& reflect) override;

		public:
			virtual const std::vector<std::string> GetDiffuseTextureList() override;
			virtual const std::vector<std::string> GetNormalTextureList() override;

		public:
			virtual void PlayAnimation(float deltaTime, bool continiousPlay) override;
			virtual void StopAnimation() override;
			virtual bool PlayAnimation(float speed, float deltaTime, int animIndex, bool continuousPlay) override;
			virtual bool PlayAnimation(float speed, float deltaTime, const std::string& animName, bool continuousPlay) override;
			virtual const std::vector<std::string>& GetClipNames() override;
			virtual void PauseAnimation() override;
			virtual void ReplayAnimation() override;
			virtual float GetCurrentFrame() override;
			virtual void SetCurrentFrame(int frame) override;
			virtual float GetMaxFrame() override;
			virtual bool GetIsPlaying() override;
			virtual const std::string& GetNowAnimationName() override;
			virtual void SetParentBone(GInterface::GraphicsRenderable* model, const std::string& boneName) override;
			virtual void DeleteParentBone() override;
			virtual DirectX::XMFLOAT4X4 GetTransformEffectedByBone() override;
			virtual DirectX::XMFLOAT4X4 GetBoneTransform(const std::string& boneName) override;
			virtual DirectX::XMFLOAT4X4 GetParentBoneOriginalTransform(const std::string& boneName) override;

		public:
			virtual bool GetPickable() override;
			virtual void SetPickable(bool tf) override;

			virtual unsigned int GetHashID() override;

		public:
			virtual bool GetInsideFrustumState() override;

		public:

		private:
			void SetHashValue(unsigned int index);

		private:
			void ConvertHashToRGBA(int hashValue);

		public:
			const DirectX::XMMATRIX GetWorldTransform();
			const std::string& GetFileName();

		public:
			virtual void SetMainCamera(ArkEngine::ICamera* camera) override;

		public:
			virtual void SetCartoonRendering(bool tf) override;
			virtual bool GetCartoonRenderingState() override;


			virtual void SetAlpha(float alpha) override;
			virtual float GetAlhpa() override;

			virtual const float GetDepth(DirectX::XMFLOAT3& cameraPos) override;

		public:
			virtual const std::string& GetName() override;
			virtual const std::string& GetEffectName() override;

			virtual std::vector<ID3D11ShaderResourceView*>& GetDiffuseSRV() override;
			virtual std::vector<ID3D11ShaderResourceView*>& GetNormalSRV() override;
			virtual std::vector<ID3D11ShaderResourceView*>& GetEmmisionSRV() override;

			virtual ArkEngine::ArkDX11::FBXAnimator* GetAnimator() override;
			virtual const DirectX::XMMATRIX GetTransformMat() override;
			virtual ArkEngine::ArkDX11::FBXMesh* GetParentMesh() override;
			virtual unsigned int GetParentBoneIndex() override;
			virtual DirectX::XMMATRIX GetParentBoneTransform() override;
			virtual const DirectX::XMFLOAT4 GetColor() override;

		private:
			std::string _simpleModelName;
			std::string _fileName;
			std::string _effectName;
			std::vector<std::string> _diffuseTextureName;
			std::vector<std::string> _normalTextureName;
			std::vector<std::string> _emssiveTextureName;

			std::vector<ID3D11ShaderResourceView*> _diffuseMapSRV;

			std::vector<ID3D11ShaderResourceView*> _normalMapSRV;

			std::vector<ID3D11ShaderResourceView*> _emissionMapSRV;

			std::vector<DirectX::XMFLOAT4X4> _boneTMList;

			std::vector<ID3D11Buffer*> _vertexBuffer;
			std::vector<ID3D11Buffer*> _indexBuffer;

		private:
			std::vector<ArkEngine::ArkDX11::Material> _material;

		private:
			UINT _totalVertexCount;
			UINT _totalIndexCount;
			ArkEngine::ArkDX11::Transform* _meshTransform;

		private:
			IDebugObject* _debugObject;

			bool _isSolid;

		private:
			bool _havePlayedAnimation;
			bool _isAnimationPlaying;
			bool _isRendering;

		private:
			bool _isPickable;
			UINT _hashValue;
			unsigned int _objectIndex;

			float _color[4];
			DirectX::XMFLOAT4 _colorVec;

			bool _haveShadow;

			bool _applyCartoonRendering;

			float _alpha;

			/// <summary>
			/// 김현재가 추가한 것들
			/// </summary>
		private:
			/// FBX 추가 김현재
			void BuildGeometryBuffersFBX(std::wstring fileName);
			void BindCacheInfo();

			/// Materials
			void ReadMaterial(std::wstring fileName);

			// .mesh, .clip 경로 
			std::wstring _modelPath;
			// xml 경로
			std::wstring _texturePath;

		private:
			std::vector<ModelMesh*> _meshes;

			// Material
			std::vector<ModelMaterial*> _materials;
		private:
			UINT _meshCount;

			UINT _boneIndexNum;

			std::vector<std::vector<Vertex>> _newVertexVector;
			std::vector<std::vector<unsigned int>> _newIndexVector;

			std::vector<Matrix> _boneTransforms;

			/// Animator
			FBXAnimator* _animator;

		private:
			FBXMesh* _parentMesh;
			DirectX::XMFLOAT4X4 _parentBoneTrasnform;
			int _parentBoneIndex;
			DirectX::XMFLOAT4X4 _transformEffectedByParent;

			/// Dissolve
		public:
			// 1일때는 정상 0에 가까워 질수록 투명해짐
			virtual void SetDissolve(float value) override;
			virtual float GetDissolveValue() override;
			virtual void SetIsDissolve(bool isDissolve) override;
			virtual bool GetIsDissolve() override;


		private:
			float _dissolveValue;
			bool _isDissolve;
		

			float timeMan = 1.0f;

		};
	}
}