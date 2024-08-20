#pragma once
#include <windows.h>
#include "DirectXMath.h"
#include "../Inc/DXTK/SimpleMath.h"

using Color = DirectX::XMFLOAT4;



using Vec2 = DirectX::XMFLOAT2;
using Vec3 = DirectX::XMFLOAT3;
using Vec4 = DirectX::XMFLOAT4;

//using Matrix = DirectX::XMMATRIX;

using Matrix = DirectX::SimpleMath::Matrix;
using Quaternion = DirectX::SimpleMath::Quaternion;


/// 이쪽을 XMFLOAT로 바꾸자
using Vector2 = DirectX::SimpleMath::Vector2;
using Vector3 = DirectX::SimpleMath::Vector3;
using Vector4 = DirectX::SimpleMath::Vector4;