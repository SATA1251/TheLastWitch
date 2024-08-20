//#include <d3d11.h>
//#include "ResourceManager.h"
//#include "ArkDevice.h"
//#include "Camera.h"
//#include "ArkEffect.h"
//#include "d3dx11effect.h"
//
//#include "ParticleEffect.h"
//
//ArkEngine::ArkDX11::ParticleEffect::ParticleEffect()
//{
//	_streamOutTech = _fx->GetTechniqueByName("StreamOutTech");
//	_drawTech = _fx->GetTechniqueByName("DrawTech");
//	
//	_viewProj = _fx->GetVariableByName("gViewProj")->AsMatrix();
//	_gameTime = _fx->GetVariableByName("gGameTime")->AsScalar();
//	_timeStep = _fx->GetVariableByName("gTimeStep")->AsScalar();
//	_eyePosW = _fx->GetVariableByName("gEyePosW")->AsVector();
//	_emitPosW = _fx->GetVariableByName("gEmitPosW")->AsVector();
//	_emitDirW = _fx->GetVariableByName("gEmitDirW")->AsVector();
//	_texArray = _fx->GetVariableByName("gTexArray")->AsShaderResource();
//	_randomTex = _fx->GetVariableByName("gRandomTex")->AsShaderResource();
//}
//
//ArkEngine::ArkDX11::ParticleEffect::~ParticleEffect()
//{
//}
//
//void ArkEngine::ArkDX11::ParticleEffect::SetViewProj(DirectX::CXMMATRIX m)
//{
//	_viewProj->SetMatrix(reinterpret_cast<const float*>(&m));
//}
//
//void ArkEngine::ArkDX11::ParticleEffect::SetGameTime(float f)
//{
//	_gameTime->SetFloat(f);
//}
//
//void ArkEngine::ArkDX11::ParticleEffect::SetTimeStep(float f)
//{
//	_timeStep->SetFloat(f);
//}
//
//void ArkEngine::ArkDX11::ParticleEffect::SetEyePosW(const DirectX::XMFLOAT3& v)
//{
//	_eyePosW->SetRawValue(&v, 0, sizeof(DirectX::XMFLOAT3));
//}
//
//void ArkEngine::ArkDX11::ParticleEffect::SetEmitPosW(const DirectX::XMFLOAT3& v)
//{
//	_emitPosW->SetRawValue(&v, 0, sizeof(DirectX::XMFLOAT3));
//}
//
//void ArkEngine::ArkDX11::ParticleEffect::SetEmitDirW(const DirectX::XMFLOAT3& v)
//{
//	_emitDirW->SetRawValue(&v, 0, sizeof(DirectX::XMFLOAT3));
//}
//
//void ArkEngine::ArkDX11::ParticleEffect::SetTexArray(ID3D11ShaderResourceView* tex)
//{
//	_texArray->SetResource(tex);
//}
//
//void ArkEngine::ArkDX11::ParticleEffect::SetRandomTex(ID3D11ShaderResourceView* tex)
//{
//	_randomTex->SetResource(tex);
//}
