#include "ToolBox.h"
#include <random>
#include "TimeManager.h"

int KunrealEngine::ToolBox::GetRandomNum(int maxNum)
{
	std::random_device rd;								// 난수 시드값 생성
	std::mt19937 generator(rd());						// 시드값을 통한 난수 생성
	std::uniform_int_distribution<> dist(0, maxNum);	// 난수의 분포 설정

	return dist(generator);								// 생성한 난수 반환
}

int KunrealEngine::ToolBox::GetRandomNum(int minNum, int maxNum)
{
	std::random_device rd;
	std::mt19937 generator(rd());
	std::uniform_int_distribution<> dist(minNum, maxNum);

	return dist(generator);
}

float KunrealEngine::ToolBox::GetRandomFloat(float minNum, float maxNum)
{
	std::random_device rd;
	std::mt19937 generator(rd());
	std::uniform_real_distribution<> dist(minNum, maxNum);

	return dist(generator);
}

float KunrealEngine::ToolBox::GetDistance(DirectX::XMFLOAT3& src, DirectX::XMFLOAT3& dst)
{
	DirectX::XMVECTOR source = DirectX::XMLoadFloat3(&src);
	DirectX::XMVECTOR destination = DirectX::XMLoadFloat3(&dst);

	DirectX::XMVECTOR distance = DirectX::XMVectorSubtract(destination, source);

	float result = DirectX::XMVectorGetX(DirectX::XMVector3Length(distance));

	return result;
}

DirectX::XMVECTOR KunrealEngine::ToolBox::GetDirectionVec(DirectX::XMFLOAT3& src, DirectX::XMFLOAT3& dst)
{
	DirectX::XMVECTOR source = DirectX::XMLoadFloat3(&src);
	DirectX::XMVECTOR destination = DirectX::XMLoadFloat3(&dst);

	DirectX::XMVECTOR result = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(destination, source));

	return result;
}

float KunrealEngine::ToolBox::GetAngle(DirectX::XMFLOAT3 src, DirectX::XMFLOAT3 dst)
{
	DirectX::XMVECTOR currentPosVec = DirectX::XMLoadFloat3(&src);
	DirectX::XMVECTOR targetPosVec = DirectX::XMLoadFloat3(&dst);

	DirectX::XMVECTOR currentForward = DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
	DirectX::XMVECTOR targetDirection = DirectX::XMVectorSubtract(targetPosVec, currentPosVec);
	targetDirection.m128_f32[1] = 0.0f;
	targetDirection = DirectX::XMVector3Normalize(targetDirection);

	// 두 벡터 간의 각도를 계산
	auto dotResult = DirectX::XMVector3Dot(currentForward, targetDirection);
	float dotProduct = DirectX::XMVectorGetX(dotResult);

	// 각도를 라디안에서 도로 변환
	float angle = acos(dotProduct);
	angle = DirectX::XMConvertToDegrees(angle);

	if (targetPosVec.m128_f32[0] > currentPosVec.m128_f32[0])
	{
		angle *= -1;
	}

	return angle;
}


float KunrealEngine::ToolBox::GetAngleWithDirection(const DirectX::XMFLOAT3& src, const DirectX::XMFLOAT3& dst, float prevAngle)
{
	auto direction = DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f);
	direction = RotateVector(direction, prevAngle);

	DirectX::XMVECTOR currentPosVec = DirectX::XMLoadFloat3(&src);
	DirectX::XMVECTOR targetPosVec = DirectX::XMLoadFloat3(&dst);

	DirectX::XMVECTOR currentForward = DirectX::XMLoadFloat3(&direction);
	DirectX::XMVECTOR targetDirection = DirectX::XMVectorSubtract(targetPosVec, currentPosVec);
	targetDirection.m128_f32[1] = 0.0f;
	targetDirection = DirectX::XMVector3Normalize(targetDirection);

	// 외적
	DirectX::XMVECTOR crossResult = DirectX::XMVector3Cross(currentForward, targetDirection);
	// 두 백터의 각이 180도보다 클때 y값이 음수로 나옴 
	float crossProduct = DirectX::XMVectorGetY(crossResult);

	// 내적을 통해 각도를 도출
	float angle = acos(DirectX::XMVectorGetX(DirectX::XMVector3Dot(currentForward, targetDirection)));
	angle = DirectX::XMConvertToDegrees(angle);

	// 두 백터의 각이 180도보다 클 경우 반대로 돌아야함
	if (crossProduct < 0)
	{
		angle *= -1;
	}

	return angle;
}

DirectX::XMFLOAT3 KunrealEngine::ToolBox::QuaternionToEulerAngles(const DirectX::XMVECTOR& quaternion)
{
	DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationQuaternion(quaternion);

	float pitch, yaw, roll;
	if (rotationMatrix.r[2].m128_f32[1] > 0.998f)
	{
		pitch = DirectX::XM_PI / 2.0f;
		yaw = atan2f(rotationMatrix.r[0].m128_f32[2], rotationMatrix.r[0].m128_f32[0]);
		roll = 0.0f;
	}
	else if (rotationMatrix.r[2].m128_f32[1] < -0.998f)
	{
		pitch = -DirectX::XM_PI / 2.0f;
		yaw = atan2f(rotationMatrix.r[0].m128_f32[2], rotationMatrix.r[0].m128_f32[0]);
		roll = 0.0f;
	}
	else
	{
		pitch = asinf(-rotationMatrix.r[2].m128_f32[1]);
		yaw = atan2f(rotationMatrix.r[0].m128_f32[1], rotationMatrix.r[1].m128_f32[1]);
		roll = atan2f(rotationMatrix.r[2].m128_f32[0], rotationMatrix.r[2].m128_f32[2]);
	}

	return DirectX::XMFLOAT3(roll, pitch, yaw);
}

DirectX::XMFLOAT3 KunrealEngine::ToolBox::RotateVector(const DirectX::XMFLOAT3& direction, float angle)
{
	DirectX::XMVECTOR currentForward = DirectX::XMLoadFloat3(&direction);

	float radians = DirectX::XMConvertToRadians(angle);

	DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationY(radians);

	DirectX::XMVECTOR rotatedVector = DirectX::XMVector3Transform(currentForward, rotationMatrix);

	DirectX::XMFLOAT3 rotatedDirection;
	DirectX::XMStoreFloat3(&rotatedDirection, rotatedVector);
	return rotatedDirection;
}


float KunrealEngine::ToolBox::CalculateAngleBetweenVectors(const DirectX::XMFLOAT3& vec1, const DirectX::XMFLOAT3& vec2)
{
	auto oneVec = DirectX::XMLoadFloat3(&vec1);
	auto anotherVec = DirectX::XMLoadFloat3(&vec2);

	// 벡터의 내적을 계산
	float dotProduct = DirectX::XMVectorGetX(DirectX::XMVector3Dot(oneVec, anotherVec));

	// 벡터의 크기를 계산
	float magnitudeVec1 = DirectX::XMVectorGetX(DirectX::XMVector3Length(oneVec));
	float magnitudeVec2 = DirectX::XMVectorGetX(DirectX::XMVector3Length(anotherVec));

	// 내적과 크기를 사용하여 코사인 값을 계산
	float cosTheta = dotProduct / (magnitudeVec1 * magnitudeVec2);

	// 코사인 값을 -1과 1 사이로 클램프
	cosTheta = std::max(-1.0f, std::min(1.0f, cosTheta));

	// 라디안 단위의 각도를 구함
	float angleInRadians = std::acos(cosTheta);

	// 각도를 도 단위로 변환
	float angleInDegrees = DirectX::XMConvertToDegrees(angleInRadians);

	// 두 벡터의 외적을 계산하여 부호를 확인하고 부호를 적용
	DirectX::XMVECTOR crossProduct = DirectX::XMVector3Cross(oneVec, anotherVec);
	DirectX::XMFLOAT3 crossProductResult;
	DirectX::XMStoreFloat3(&crossProductResult, crossProduct);

	if (crossProductResult.y < 0) {
		angleInDegrees = -angleInDegrees;
	}

	return angleInDegrees;
}

DirectX::XMFLOAT3 KunrealEngine::ToolBox::RotateVector(const DirectX::XMFLOAT3& direction, DirectX::XMVECTOR& quaternion)
{
	DirectX::XMVECTOR vec = XMLoadFloat3(&direction);

	quaternion = DirectX::XMQuaternionNormalize(quaternion);

	DirectX::XMVECTOR rotatedVec = DirectX::XMVector3Rotate(vec, quaternion);

	DirectX::XMFLOAT3 rotatedDirection;
	XMStoreFloat3(&rotatedDirection, rotatedVec);

	return rotatedDirection;
}

void KunrealEngine::ToolBox::CalculateParabolaPath(const DirectX::XMFLOAT3& src, const DirectX::XMFLOAT3& dst, float duration, float gravity, std::vector<DirectX::XMFLOAT3>& path)
{
	float t = 0.0f;

	// 중력가속도
	DirectX::XMFLOAT3 velocity = { (dst.x - src.x) / duration, (dst.y - src.y) / duration - 0.5f * gravity * duration, (dst.z - src.z) / duration };

	while (t <= duration)
	{
		float x = src.x + velocity.x * t;
		float y = src.y + velocity.y * t + 0.5f * gravity * t * t;
		float z = src.z + velocity.z * t;

		path.emplace_back(DirectX::XMFLOAT3{ x, y, z });
		t += TimeManager::GetInstance().GetDeltaTime();
	}
}

// 선형 보간 함수
DirectX::XMFLOAT3 KunrealEngine::ToolBox::Lerp(DirectX::XMFLOAT3 p0, DirectX::XMFLOAT3 p1, float t)
{
	DirectX::XMFLOAT3 p =
	{
		p0.x + t * (p1.x - p0.x),
		p0.y + t * (p1.y - p0.y),
		p0.z + t * (p1.z - p0.z)
	};

	return p;
}

DirectX::XMFLOAT3 KunrealEngine::ToolBox::XMFLOAT3Add(const DirectX::XMFLOAT3& a, const DirectX::XMFLOAT3& b)
{
	DirectX::XMFLOAT3 result(a.x + b.x, a.y + b.y, a.z + b.z);

	return result;
}

DirectX::XMFLOAT3 KunrealEngine::ToolBox::Bezier(DirectX::XMFLOAT3 startPoint, DirectX::XMFLOAT3 p1, DirectX::XMFLOAT3 p2, DirectX::XMFLOAT3 endPoint, float t)
{
	float u = 1.0f - t;
	float tt = t * t;
	float ttt = tt * t;
	float uu = u * u;
	float uuu = uu * u;

	DirectX::XMFLOAT3 p =
	{
		uuu * startPoint.x + 3 * uu * t * p1.x + 3 * u * tt * p2.x + ttt * endPoint.x,
		uuu * startPoint.y + 3 * uu * t * p1.y + 3 * u * tt * p2.y + ttt * endPoint.y,
		uuu * startPoint.z + 3 * uu * t * p1.z + 3 * u * tt * p2.z + ttt * endPoint.z,
	};

	return p;
}


DirectX::XMFLOAT3 KunrealEngine::ToolBox::LogarithmicInterpolation(const DirectX::XMFLOAT3& startPos, const DirectX::XMFLOAT3& endPos, float t)
{
	// a점과 b점 사이의 벡터를 계산
	DirectX::XMVECTOR start = DirectX::XMLoadFloat3(&startPos);
	DirectX::XMVECTOR end = DirectX::XMLoadFloat3(&endPos);
	DirectX::XMVECTOR delta = DirectX::XMVectorSubtract(end, start);

	// 로그 함수 형태의 스칼라 값을 계산
	float logValue = std::log1p(t);

	// Z축 값의 진행 정도를 계산
	float zStart = startPos.z;
	float zEnd = endPos.z;
	float zProgress = (zStart != zEnd) ? ((std::log1p(t) - std::log1p(0.0f)) / (std::log1p(1.0f) - std::log1p(0.0f))) : 0.0f;

	// Y축 값 조정: Z 진행 정도의 제곱근을 사용하여 Y값 조정
	float yStart = startPos.y;
	float yEnd = endPos.y;
	float newY = yStart + (yEnd - yStart) * std::pow(zProgress, 0.5f);

	// 로그 스칼라 값을 사용하여 새로운 위치를 계산
	DirectX::XMVECTOR newPosition = DirectX::XMVectorMultiplyAdd(DirectX::XMVectorReplicate(logValue), delta, start);

	DirectX::XMFLOAT3 tempResult;
	XMStoreFloat3(&tempResult, newPosition);

	// 최종 결과 저장
	DirectX::XMFLOAT3 result = { tempResult.x, newY, tempResult.z };
	return result;
}


DirectX::XMFLOAT3 KunrealEngine::ToolBox::XMFLOAT3Subtract(const DirectX::XMFLOAT3& a, const DirectX::XMFLOAT3& b)
{
	DirectX::XMFLOAT3 result(a.x - b.x, a.y - b.y, a.z - b.z);

	return result;
}