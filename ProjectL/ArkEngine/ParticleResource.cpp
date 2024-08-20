#include <d3d11.h>
#include "ResourceManager.h"
#include "ParticleResource.h"

ArkEngine::ParticleResource::ParticleResource(const std::string& particleName, ID3D11ShaderResourceView* texArray, ID3D11Buffer* initVB)
	: _particleName(particleName), _texArray(texArray), _initVB(initVB)
{
	
}

ArkEngine::ParticleResource::~ParticleResource()
{
	_texArray->Release();

	_initVB->Release();
}

ID3D11ShaderResourceView* ArkEngine::ParticleResource::GetTexArray()
{
	return _texArray;
}

ID3D11Buffer* ArkEngine::ParticleResource::GetInitVB()
{
	return _initVB;
}

void ArkEngine::ParticleResource::SetTexArray(ID3D11ShaderResourceView* texArray)
{
	_texArray = texArray;
}

void ArkEngine::ParticleResource::SetInitVB(ID3D11Buffer* initVB)
{
	_initVB = initVB;
}
