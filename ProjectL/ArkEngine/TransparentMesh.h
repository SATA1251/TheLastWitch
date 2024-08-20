#pragma once
#include  "GraphicsTransParentMesh.h"

struct ID3DX11Effect;
struct ID3D11ShaderResourceView;
struct ID3DX11EffectTechnique;
struct ID3DX11EffectMatrixVariable;
struct ID3DX11EffectVectorVariable;
struct ID3DX11EffectScalarVariable;
struct ID3D11Buffer;

namespace DirectX
{
	struct XMFLOAT4;
	struct XMFLOAT4X4;
}

namespace ArkEngine
{
	class ICamera;

	namespace ArkDX11
	{
		class ArkTexture;
		class ArkEffect;
		class ArkDevice;
		class Transform;
		class deferredBuffer;
	}
}
namespace ArkEngine
{
	namespace ArkDX11
	{
		class TransparentMesh : public GInterface::GraphicsTransparentMesh
		{
		public:
			TransparentMesh(const std::string& objectName, const std::string& textureName, float transParency, bool isCircle = false);
			~TransparentMesh();

		public:
			virtual void SetTexture(const std::string& textureName) override;

			virtual bool RenderWithTimer(float deltaTime, float timer) override;

			virtual void Reset() override;

			virtual void SetRenderType(unsigned int index) override;

			virtual void SetTransform(const DirectX::XMFLOAT4X4& matrix) override;

			// 도넛을 위한 레인지 설정
			virtual void SetExceptRange(const DirectX::XMFLOAT3& center, float range) override;

			virtual void SetStartFlag(bool tf) override;

			virtual void Delete() override;

			virtual void SetDecal(bool tf) override;

			virtual void SetInfinite(bool tf) override;

			virtual bool GetInfiniteState() override;

			virtual void ApplyPattern(bool tf) override;

			virtual bool GetIsApplyPattern() override;

			virtual float GetRenderTimer() override;

		public:
			void Initialize();
			void Update(ArkEngine::ICamera* p_Camera);
			void Render(std::vector<DirectX::XMFLOAT4X4>& worldVec);
			void Finalize();

			float GetTransParency();
			void UpPosition(float up);

			ID3D11ShaderResourceView* GetTexture();

			int GetIndex();

			float GetTimer();

		private:
			void BuildGeomtryBuffers();
			void SetEffect();

		private:
			std::string _objectName;
			std::string _textureName;

			float _transParency;

			ArkEngine::ArkDX11::ArkEffect* _arkEffect;
			ID3DX11Effect* _effect;

			ArkEngine::ArkDX11::ArkDevice* _arkDevice;

			ArkEngine::ArkDX11::Transform* _meshTransform;

			ID3D11Buffer* _vertexBuffer;
			ID3D11Buffer* _indexBuffer;

			unsigned int _totalIndexCount;

			ID3DX11EffectTechnique* _tech;
			ID3DX11EffectMatrixVariable* _fxWorld;
			ID3DX11EffectMatrixVariable* _fxWorldViewProj;

			DirectX::XMFLOAT4X4 _world;
			DirectX::XMFLOAT4X4 _view;
			DirectX::XMFLOAT4X4 _proj;

			ID3DX11EffectShaderResourceVariable* _texture;
			ID3D11ShaderResourceView* _diffuseMapSRV;

			ID3DX11EffectScalarVariable* _fxTransParency;

			ID3DX11EffectScalarVariable* _fxTime;

			ID3DX11EffectScalarVariable* _fxIndex;

			ID3DX11EffectVectorVariable* _fxDonutCenter;

			ID3DX11EffectScalarVariable* _fxDonutRange;

			bool _isCircle;

			float _timer;

			unsigned int _renderType;

			float _renderTime;

			bool _isRenderFinsh;

			bool _isRenderStart;

			DirectX::XMFLOAT3 _donutCenter;

			float _donutRange;

			int _index;

			bool _isApplyDecal;

			bool _isInfinite;

			bool _isApplyPattern;
		};
	}
}