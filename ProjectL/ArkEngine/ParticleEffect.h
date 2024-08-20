//#pragma once
//
//namespace ArkEngine
//{
//	namespace ArkDX11
//	{
//		class ParticleEffect : public ArkEffect
//		{
//		public:
//			ParticleEffect();
//			~ParticleEffect();
//
//		public:
//			void SetViewProj(DirectX::CXMMATRIX m);
//
//			void SetGameTime(float f);
//			void SetTimeStep(float f);
//
//			void SetEyePosW(const DirectX::XMFLOAT3& v);
//			void SetEmitPosW(const DirectX::XMFLOAT3& v);
//			void SetEmitDirW(const DirectX::XMFLOAT3& v);
//
//			void SetTexArray(ID3D11ShaderResourceView* tex);
//			void SetRandomTex(ID3D11ShaderResourceView* tex);
//
//			ID3DX11EffectTechnique* _streamOutTech;
//			ID3DX11EffectTechnique* _drawTech;
//
//			ID3DX11EffectMatrixVariable* _viewProj;
//			ID3DX11EffectScalarVariable* _gameTime;
//			ID3DX11EffectScalarVariable* _timeStep;
//			ID3DX11EffectVectorVariable* _eyePosW;
//			ID3DX11EffectVectorVariable* _emitPosW;
//			ID3DX11EffectVectorVariable* _emitDirW;
//
//			ID3DX11EffectShaderResourceVariable* _texArray;
//			ID3DX11EffectShaderResourceVariable* _randomTex;
//
//
//		private:
//			ID3DX11Effect* _fx;
//		};
//
//	}
//}
//
