#pragma once
#include <d3d11.h>
#include <dxgi.h>
#include <DirectXMath.h>


struct BoundingSphere
{
	BoundingSphere() : Center(0.0f, 0.0f, 0.0f), Radius(0.0f) {}
	DirectX::XMFLOAT3 Center;
	float Radius;
};

namespace ArkEngine
{
	class ShadowMap;
}

class ShadowApp
{
public:
	ShadowApp();
	~ShadowApp();

public:
	void UpdateScene(float dt);
	void BuildShadowTransform();

private:
	ArkEngine::ShadowMap* _smap;
	BoundingSphere mSceneBounds;
};

