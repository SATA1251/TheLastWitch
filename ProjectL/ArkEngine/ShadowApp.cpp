#include "ShadowMap.h"
#include "ShadowApp.h"

ShadowApp::ShadowApp()
{
}

ShadowApp::~ShadowApp()
{
}

void ShadowApp::UpdateScene(float dt)
{
	//// 광원을 움직인다
	//// mLightRotationAngle += 0.1f * dt;
	//DirectX::XMMATRIX R = XMMatrixRotationY(mLightRotationAngle);
	//for (int i = 0; i < 3; ++i)
	//{
	//	DirectX::XMVECTOR lightDir = DirectX::XMLoadFloat3(&mOriginalLightDir[i]);
	//	lightDir = XMVector3TransformNormal()
	//}
}

void ShadowApp::BuildShadowTransform()
{
	// 첫번째의 주 광원만 그림자를 드리운다
	//DirectX::XMVECTOR lightDir = DirectX::XMLoadFloat3()
}
