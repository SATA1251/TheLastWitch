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
	//// ������ �����δ�
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
	// ù��°�� �� ������ �׸��ڸ� �帮���
	//DirectX::XMVECTOR lightDir = DirectX::XMLoadFloat3()
}
