/// <summary>
/// 
/// 인스턴싱을 활용한 렌더링을 실시하기 위함
/// 
/// 윤종화
/// </summary>

#pragma once
#include <string>
#include <vector>
#include <d3d11.h>

namespace ArkEngine
{
	class IRenderable;
	class ICamera;

	namespace ArkDX11
	{
		class ArkEffect;
		class ArkDevice;
		struct Material;

		//class ArkTexture;
	}
}

struct ID3DX11Effect;
struct ID3DX11EffectTechnique;
struct ID3DX11EffectMatrixVariable;
struct ID3DX11EffectVariable;
struct ID3DX11EffectShaderResourceVariable;
struct ID3DX11EffectVectorVariable;
struct ID3DX11EffectScalarVariable;
struct ID3D11ShaderResourceView;

namespace ArkEngine
{
	class MeshRenderer
	{
	public:
		MeshRenderer(IRenderable* mesh);
		~MeshRenderer();

	public:
		void AddMeshInList(IRenderable* mesh);
		void DeleteMeshInList(IRenderable* mesh);

	public:
		void Render();

		void RenderForward();

		void ShadowRender();

	public:
		void SetMainCamera(ICamera* mainCamera);

		bool GetAlphaExist();

		const float GetDepth();

		const std::vector<IRenderable*>& GetMeshList();

	public:
		const std::string& GetName();

	private:
		void SetEffect(IRenderable* mesh);

	private:
		void Initialize(IRenderable* mesh);

	private:
		std::string _fileName;
		std::string _effectName;

	private:
		std::vector<IRenderable*> _meshList;

	private:
		std::vector<IRenderable*> _renderList;
		std::vector<IRenderable*> _shadowList;
		std::vector<IRenderable*> _transparentList;

		// 오브젝트마다 다른 것
	private:
		std::vector<DirectX::XMFLOAT4> _colorList;
		std::vector<DirectX::XMFLOAT4X4> _worldList;
		std::vector<DirectX::XMFLOAT4X4> _worldInvList;
		std::vector<DirectX::XMFLOAT4X4> _worldViewProjList;

		// 오브젝트마다 일치하는 것
	private:
		std::vector<ID3D11Buffer*> _vertexBuffer;
		std::vector<ID3D11Buffer*> _indexBuffer;
		std::vector<ArkEngine::ArkDX11::Material> _material;
		std::vector<ModelMesh*> _meshes;

	private:
		ID3DX11Effect* _effect;
		ID3DX11EffectTechnique* _tech;
		ID3DX11EffectMatrixVariable* _fxBoneTransforms;

		ID3DX11EffectMatrixVariable* _fxWorld;
		ID3DX11EffectMatrixVariable* _fxWorldInvTranspose;
		ID3DX11EffectMatrixVariable* _fxworldViewProj;
		ID3DX11EffectMatrixVariable* _fxTexTransform;
		ID3DX11EffectVariable* _fxMaterial;

		ID3DX11EffectShaderResourceVariable* _diffuseMap;
		ID3DX11EffectShaderResourceVariable* _normalMap;
		ID3DX11EffectShaderResourceVariable* _emissionMap;

		ID3DX11EffectVectorVariable* _fxColor;
		ID3DX11EffectScalarVariable* _fxCartoon;

		ID3DX11EffectScalarVariable* _fxAlpha;

		ID3DX11EffectScalarVariable* _fxApplayDecal;
		std::vector<float> _alphaList;

		bool _isTransparentExist;

		bool _isApplyingDecal;

	private:
		// 기초 세팅 가져오기
		ArkEngine::ArkDX11::ArkDevice* _arkDevice;
		ArkEngine::ArkDX11::ArkEffect* _arkEffect;

		ICamera* _mainCamera;

		// Dissolve Effect
	public:
		// 1일때는 정상 0에 가까워 질수록 투명해짐
		void SetDissolve();
		float GetDissolveValue();
		void SetIsDissolve();

	private:
		void SetDissolveSRV(float value);
		void SetDissolveTexture();
		void SetIsDissolveSRV(bool isDissolve);

		ID3DX11EffectShaderResourceVariable* _noiseMap;
		ID3DX11EffectShaderResourceVariable* _burnGradation;

		ID3D11ShaderResourceView* _noiseMapSRV;
		ID3D11ShaderResourceView* _burnGradationSRV;

		std::string _noiseMapName;
		std::string _burnGradationName;

		ID3DX11EffectScalarVariable* _dissolveValueEffect;
		ID3DX11EffectScalarVariable* _isDisolveEffect;

		float _dissolveValue;

		float timeMan = 1.0f;

		bool _isDissolve;
	};
}