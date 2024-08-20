#include <d3dcommon.h>
#include <d3dcompiler.h>
#include <assert.h>
#include "ResourceManager.h"
#include "ArkDevice.h"
#include "d3dx11effect.h"
#include "ArkEffect.h"

ArkEngine::ArkDX11::ArkEffect::ArkEffect(const char* effectName)
	: _effect(nullptr), _inputLayOut(nullptr), _vertexDescIndex(0)
{
	CreateEffect(effectName);
}

ArkEngine::ArkDX11::ArkEffect::~ArkEffect()
{
	_inputLayOut->Release();
	_effect->Release();
}

ID3DX11Effect* ArkEngine::ArkDX11::ArkEffect::GetEffect()
{
	return _effect;
}

ID3D11InputLayout* ArkEngine::ArkDX11::ArkEffect::GetInputLayOut()
{
	return _inputLayOut;
}

void ArkEngine::ArkDX11::ArkEffect::Load(const char* fileName)
{
	ResourceManager::GetInstance()->SetResource(fileName, this);
}

void ArkEngine::ArkDX11::ArkEffect::UnLoad(const char* fileName)
{
	ResourceManager::GetInstance()->DeleteResource(fileName);
}

void ArkEngine::ArkDX11::ArkEffect::CreateEffect(const char* effectName)
{
	ID3DBlob* compiledShader;
	ID3DBlob* compilationMsgs;

	int length = MultiByteToWideChar(CP_UTF8, 0, effectName, -1, NULL, 0);
	wchar_t* unicodeString = new wchar_t[length];
	MultiByteToWideChar(CP_UTF8, 0, effectName, -1, unicodeString, length);

	LPCWSTR tempEffectName = reinterpret_cast<LPCWSTR>(unicodeString);

	D3DCompileFromFile(tempEffectName, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, NULL, "fx_5_0", D3DCOMPILE_DEBUG, 0, &compiledShader, &compilationMsgs);
	auto arkDevice = ResourceManager::GetInstance()->GetResource<ArkEngine::ArkDX11::ArkDevice>("Device");
	D3DX11CreateEffectFromMemory(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(), 0, arkDevice->GetDevice(), &_effect);
	
	ID3DX11EffectTechnique* p_Tech;
	p_Tech = _effect->GetTechniqueByIndex(0);

	D3DX11_PASS_DESC passDesc;
	p_Tech->GetPassByIndex(0)->GetDesc(&passDesc);

	auto vertexDesc = GetInputDesc(effectName);

	assert(_vertexDescIndex != 0);
	arkDevice->GetDevice()->CreateInputLayout(vertexDesc, _vertexDescIndex, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &_inputLayOut);

	Load(effectName);

	// ¸¶¹«¸®
	_vertexDescIndex = 0;

	delete vertexDesc;
	
	delete[] unicodeString;

	compilationMsgs->Release();
	compiledShader->Release();
}

D3D11_INPUT_ELEMENT_DESC* ArkEngine::ArkDX11::ArkEffect::GetInputDesc(const char* effectName)
{
	std::string stringEffect = effectName;
	D3D11_INPUT_ELEMENT_DESC* vertexDesc = nullptr;

	if (stringEffect.find("color") != std::string::npos)
	{
		vertexDesc = new D3D11_INPUT_ELEMENT_DESC[2];
		vertexDesc[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		vertexDesc[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 };

		_vertexDescIndex = 2;
	}
	else if (stringEffect.find("BasicTex") != std::string::npos)
	{
		vertexDesc = new D3D11_INPUT_ELEMENT_DESC[4];
		vertexDesc[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		vertexDesc[1] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		vertexDesc[2] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		vertexDesc[3] = { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 };

		_vertexDescIndex = 4;
	}
	else if (stringEffect.find("Skinning") != std::string::npos)
	{
		vertexDesc = new D3D11_INPUT_ELEMENT_DESC[6];
		vertexDesc[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		vertexDesc[1] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		vertexDesc[2] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		vertexDesc[3] = { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		vertexDesc[4] = { "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		vertexDesc[5] = { "BLENDINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, 56, D3D11_INPUT_PER_VERTEX_DATA, 0 };

		_vertexDescIndex = 6;
	}
	else if (stringEffect.find("final") != std::string::npos)
	{
		vertexDesc = new D3D11_INPUT_ELEMENT_DESC[2];
		vertexDesc[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		vertexDesc[1] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 };

		_vertexDescIndex = 2;
	}
	else if (stringEffect.find("Sky") != std::string::npos)
	{
		vertexDesc = new D3D11_INPUT_ELEMENT_DESC[1];
		vertexDesc[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };

		_vertexDescIndex = 1;
	}
	else if (stringEffect.find("UI") != std::string::npos)
	{
		vertexDesc = new D3D11_INPUT_ELEMENT_DESC[2];
		vertexDesc[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		vertexDesc[1] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 };

		_vertexDescIndex = 2;
	}
	else if (stringEffect.find("Particle") != std::string::npos)
	{
		vertexDesc = new D3D11_INPUT_ELEMENT_DESC[5];
		vertexDesc[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		vertexDesc[1] = { "VELOCITY", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		vertexDesc[2] = { "SIZE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		vertexDesc[3] = { "AGE", 0, DXGI_FORMAT_R32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		vertexDesc[4] = { "TYPE", 0, DXGI_FORMAT_R32_UINT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 };

		_vertexDescIndex = 5;
	}
	else if (stringEffect.find("Transparent") != std::string::npos)
	{
		vertexDesc = new D3D11_INPUT_ELEMENT_DESC[2];
		vertexDesc[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		vertexDesc[1] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 };

		_vertexDescIndex = 2;
	}
	else if (stringEffect.find("Bloom") != std::string::npos)
	{
		vertexDesc = new D3D11_INPUT_ELEMENT_DESC[2];
		vertexDesc[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		vertexDesc[1] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 };

		_vertexDescIndex = 2;
	};

	return vertexDesc;
}
