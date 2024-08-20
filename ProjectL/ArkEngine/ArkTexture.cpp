#include <filesystem>
#include "DDSTextureLoader.h"
#include "DXTK/DirectXTex.h"
#include "ResourceManager.h"
#include "ArkDevice.h"
#include "ArkTexture.h"

ArkEngine::ArkDX11::ArkTexture::ArkTexture(const char* textureName)
	: _diffuseMapSRV(nullptr)
{
	std::string tempString = textureName;

	if (tempString.find("dds") != std::string::npos)
	{
		CreateDDSTexture(textureName);
	}
	else if (tempString.find("tga") != std::string::npos)
	{
		CreateTGATexture(textureName);
	}
	else
	{
		CreateTexture(textureName);
	}
}

ArkEngine::ArkDX11::ArkTexture::~ArkTexture()
{
	_diffuseMapSRV->Release();
}

void ArkEngine::ArkDX11::ArkTexture::Load(const char* fileName)
{
	ResourceManager::GetInstance()->SetResource(fileName, this);
}

void ArkEngine::ArkDX11::ArkTexture::UnLoad(const char* fileName)
{
	ResourceManager::GetInstance()->DeleteResource(fileName);
}

ID3D11ShaderResourceView* ArkEngine::ArkDX11::ArkTexture::GetDiffuseMapSRV()
{
	return _diffuseMapSRV;
}

void ArkEngine::ArkDX11::ArkTexture::CreateDDSTexture(const char* textureName)
{
	ID3D11Resource* _textureResource = nullptr;

	auto device = ResourceManager::GetInstance()->GetResource<ArkEngine::ArkDX11::ArkDevice>("Device")->GetDevice();

	int length = MultiByteToWideChar(CP_UTF8, 0, textureName, -1, NULL, 0);
	wchar_t* unicodeString = new wchar_t[length];
	MultiByteToWideChar(CP_UTF8, 0, textureName, -1, unicodeString, length);

	LPCWSTR tempTextureName = reinterpret_cast<LPCWSTR>(unicodeString);

	DirectX::CreateDDSTextureFromFile(device, tempTextureName, &_textureResource, &_diffuseMapSRV);

	ID3D11Texture2D* texture2D = nullptr;
	_textureResource->QueryInterface(__uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&texture2D));

	D3D11_TEXTURE2D_DESC desc;
	texture2D->GetDesc(&desc);
	_textureSize.x = desc.Width;
	_textureSize.y = desc.Height;

	texture2D->Release();

	Load(textureName);

	_textureResource->Release();
}

void ArkEngine::ArkDX11::ArkTexture::CreateTexture(const char* textureName)
{
	auto device = ResourceManager::GetInstance()->GetResource<ArkEngine::ArkDX11::ArkDevice>("Device")->GetDevice();

	std::string tempString = textureName;
	std::wstring wideStr = std::filesystem::path(tempString).wstring();

	const wchar_t* wcharPtr = wideStr.c_str();

	// 텍스처 로드에 사용할 이미지를 로드합니다.
	DirectX::TexMetadata metadata;
	DirectX::ScratchImage scratchImage;
	HRESULT hr = DirectX::LoadFromWICFile(
		wcharPtr, // 파일 경로
		DirectX::WIC_FLAGS_NONE,
		&metadata,
		scratchImage
	);
	if (FAILED(hr)) {
		// 실패 처리
		return;
	}

	// 밉맵 체인 생성
	DirectX::ScratchImage mipChain;
	hr = DirectX::GenerateMipMaps(scratchImage.GetImages(), scratchImage.GetImageCount(), scratchImage.GetMetadata(), DirectX::TEX_FILTER_DEFAULT, 0, mipChain);
	if (FAILED(hr)) {
		// 실패 처리
		return;
	}

	// 텍스처 설명 생성
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = metadata.width;
	textureDesc.Height = metadata.height;
	textureDesc.MipLevels = static_cast<UINT>(mipChain.GetMetadata().mipLevels);
	textureDesc.ArraySize = 1;
	textureDesc.Format = metadata.format;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	// 텍스처 생성
	ID3D11Texture2D* texture = nullptr;
	D3D11_SUBRESOURCE_DATA* initData = new D3D11_SUBRESOURCE_DATA[textureDesc.MipLevels];
	const DirectX::Image* images = mipChain.GetImages();
	for (size_t i = 0; i < textureDesc.MipLevels; ++i) {
		initData[i].pSysMem = images[i].pixels;
		initData[i].SysMemPitch = static_cast<UINT>(images[i].rowPitch);
		initData[i].SysMemSlicePitch = static_cast<UINT>(images[i].slicePitch);
	}

	hr = device->CreateTexture2D(&textureDesc, initData, &texture);
	delete[] initData;

	if (FAILED(hr)) {
		// 실패 처리
		return;
	}

	// SRV 생성
	ID3D11ShaderResourceView* srv;
	hr = device->CreateShaderResourceView(texture, nullptr, &srv);
	if (FAILED(hr)) {
		// 실패 처리
		texture->Release();
		return;
	}

	// 텍스처 해제
	texture->Release();

	// 생성된 SRV를 사용하도록 설정
	_diffuseMapSRV = srv;

	_textureSize.x = textureDesc.Width;
	_textureSize.y = textureDesc.Height;

	// 나머지 로드 및 초기화 코드
	Load(textureName);
}

void ArkEngine::ArkDX11::ArkTexture::CreateTGATexture(const char* textureName)
{
	auto device = ResourceManager::GetInstance()->GetResource<ArkEngine::ArkDX11::ArkDevice>("Device")->GetDevice();

	std::string tempString = textureName;
	std::wstring wideStr = std::filesystem::path(tempString).wstring();

	const wchar_t* wcharPtr = wideStr.c_str();

	// 텍스처 로드에 사용할 이미지를 로드합니다.
	DirectX::TexMetadata metadata;
	DirectX::ScratchImage scratchImage;
	HRESULT hr = DirectX::LoadFromTGAFile(
		wcharPtr, // 파일 경로
		&metadata,
		scratchImage
	);
	if (FAILED(hr)) {
		// 실패 처리
		return;
	}

	// 밉맵 체인 생성
	DirectX::ScratchImage mipChain;
	hr = DirectX::GenerateMipMaps(scratchImage.GetImages(), scratchImage.GetImageCount(), scratchImage.GetMetadata(), DirectX::TEX_FILTER_DEFAULT, 0, mipChain);
	if (FAILED(hr)) {
		// 실패 처리
		return;
	}

	// 텍스처 설명 생성
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = metadata.width;
	textureDesc.Height = metadata.height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = metadata.format;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	// 텍스처 생성
	ID3D11Texture2D* texture = nullptr;
	D3D11_SUBRESOURCE_DATA* initData = new D3D11_SUBRESOURCE_DATA[textureDesc.MipLevels];
	const DirectX::Image* images = mipChain.GetImages();
	for (size_t i = 0; i < textureDesc.MipLevels; ++i) {
		initData[i].pSysMem = images[i].pixels;
		initData[i].SysMemPitch = static_cast<UINT>(images[i].rowPitch);
		initData[i].SysMemSlicePitch = static_cast<UINT>(images[i].slicePitch);
	}

	hr = device->CreateTexture2D(&textureDesc, initData, &texture);

	delete[] initData;

	if (FAILED(hr)) {
		// 실패 처리
		return;
	}

	// SRV 생성
	ID3D11ShaderResourceView* srv;
	hr = device->CreateShaderResourceView(texture, nullptr, &srv);
	if (FAILED(hr)) {
		// 실패 처리
		texture->Release();
		return;
	}

	// 텍스처 해제
	texture->Release();

	// 생성된 SRV를 사용하도록 설정
	_diffuseMapSRV = srv;

	_textureSize.x = textureDesc.Width;
	_textureSize.y = textureDesc.Height;

	// 나머지 로드 및 초기화 코드
	Load(textureName);
}


const DirectX::XMUINT2& ArkEngine::ArkDX11::ArkTexture::GetTextureSize()
{
	return _textureSize;
}
