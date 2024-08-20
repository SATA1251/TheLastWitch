#include "ParsingStructs.h"
#include "ResourceManager.h"
#include "../FbxLoader/FileUtils.h"
#include "../FbxLoader/Utils.h"
#include "ModelAnimation.h"
#include "FBXAnimator.h"

ArkEngine::ArkDX11::FBXAnimator::FBXAnimator()
	:_animationIndex(0), _anim(nullptr), _animTransforms(0),
	_frame(nullptr), _animBones(0),
	_invGlobal(),
	_animPath(L"Resources/Models/"),
	_boneTransformMatrix(0), _frameCount(0.0f),
	_isPause(false)
{
	_boneTransformMatrix.resize(150);

	for (auto& index : _boneTransformMatrix)
	{
		DirectX::XMStoreFloat4x4(&index, DirectX::XMMatrixIdentity());
	}
}

ArkEngine::ArkDX11::FBXAnimator::~FBXAnimator()
{
	_animationIndex.clear();
	_boneTransformMatrix.clear();
}

bool ArkEngine::ArkDX11::FBXAnimator::CheckClipFile(std::string fileName)
{
	_animationClips.clear();	// Ŭ�� �̸��� �ʱ�ȭ

	std::string pathName;
	size_t position = fileName.find("/");
	if (position != std::string::npos)
	{
		pathName = fileName.substr(0, position);
	}

	std::string finalPath = "Resources/Models/" + pathName;

	for (const auto& entry : std::filesystem::directory_iterator(finalPath))
	{
		if (entry.is_regular_file() && entry.path().extension() == ".clip")
		{
			auto clipName = entry.path().filename().string();

			size_t position = clipName.find(".");
			if (position != std::string::npos)
			{
				clipName = clipName.substr(0, position);
			}
			std::string finalClipName = pathName + "/" + clipName;
			std::wstring finalClipNameW = std::wstring(finalClipName.begin(), finalClipName.end());
			ReadAnimData(finalClipNameW);

			_animationClips.push_back(clipName);	// ������ ã�Ҵٸ� Ŭ�� �̸��� �����̳ʿ� �߰�
		}
	}

	if (_animationClips.empty())
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool ArkEngine::ArkDX11::FBXAnimator::PlayAnimationOnce(float speed, float deltaTime, int animIndex)
{
	_anim = GetAnimationByIndex(animIndex);

	if (_anim == nullptr)
	{
		return false;
	}

	if (_frameCount >= _anim->duration-1)
	{
		return false;
	}

	_isPause = false;
	UpdateAnimTransforms(speed, deltaTime, animIndex);
	
	return true;
}

bool ArkEngine::ArkDX11::FBXAnimator::PlayAnimationOnce(float speed, float deltaTime, std::string animName)
{
	_anim = GetAnimationByName(animName);

	if (_anim == nullptr)
	{
		return false;
	}

	if (_frameCount >= _anim->duration-1)
	{
		return false;
	}

	_isPause = false;

	UpdateAnimTransforms(speed, deltaTime, animName);

	return true;
}

bool ArkEngine::ArkDX11::FBXAnimator::PlayAnimationContinuous(float speed, float deltaTime, int animIndex)
{
	_anim = GetAnimationByIndex(animIndex);

	if (_anim == nullptr)
	{
		return false;
	}

	if (_frameCount >= _anim->duration-1)
	{
		_frameCount = 0.0f;
	}

	_isPause = false;

	UpdateAnimTransforms(speed, deltaTime, animIndex);

	return true;
}

bool ArkEngine::ArkDX11::FBXAnimator::PlayAnimationContinuous(float speed, float deltaTime, std::string animName)
{
	_anim = GetAnimationByName(animName);

	if (_anim == nullptr)
	{
		return false;
	}

	if (_frameCount >= _anim->duration-1)
	{
		_frameCount = 0.0f;
	}

	_isPause = false;

	UpdateAnimTransforms(speed, deltaTime, animName);

	return true;
}

// ������ �۵� ���ϴϱ� �ٲ�����
void ArkEngine::ArkDX11::FBXAnimator::StopFBXAnimation()
{
	_isPause = true;
	_frameCount = 0.0f;
}

const std::vector<std::string>& ArkEngine::ArkDX11::FBXAnimator::GetClipNames()
{
	if (!_animationClips.empty())
	{
		return this->_animationClips;
	}
}

void ArkEngine::ArkDX11::FBXAnimator::SetCurrentFrame(int frame)
{
	_frameCount = frame;
}

float ArkEngine::ArkDX11::FBXAnimator::GetCurrentFrame()
{
	return _frameCount;
}

float ArkEngine::ArkDX11::FBXAnimator::GetMaxFrame()
{
	return _anim->duration - 1.0f;
}

const std::string& ArkEngine::ArkDX11::FBXAnimator::GetAnimationName()
{
	if (_anim != nullptr)
	{
		return _anim->name;
	}
	else
	{
		std::string temp = "";
		return temp;
	}
}

const int ArkEngine::ArkDX11::FBXAnimator::GetIndexOfBone(const std::string& boneName)
{
	auto bone = GetBonesByName(boneName);

	auto it = std::find(_animBones.begin(), _animBones.end(), bone);

	if (it == _animBones.end())
	{
		return -1;
	}
	else
	{
		auto index = std::distance(_animBones.begin(), it);
		return index;
	}
}

const DirectX::XMFLOAT4X4 ArkEngine::ArkDX11::FBXAnimator::GetBoneTransform(const std::string& boneName)
{
	auto index = GetIndexOfBone(boneName);

	return _animBones[index]->transform;
}

const DirectX::XMFLOAT4X4 ArkEngine::ArkDX11::FBXAnimator::GetBoneAnimation(int boneIndex)
{
	if (_boneTransformMatrix.size() > boneIndex)
	{
		return _boneTransformMatrix[boneIndex];
	}
	else
	{
		DirectX::XMFLOAT4X4 mat;
		DirectX::XMStoreFloat4x4(&mat, DirectX::XMMatrixIdentity());

		return mat;
	}
}

const DirectX::XMFLOAT4X4 ArkEngine::ArkDX11::FBXAnimator::GetBoneTransform(int boneIndex)
{
	return _animBones[boneIndex]->transform;
}

void ArkEngine::ArkDX11::FBXAnimator::PauseFBXAnimation()
{
	_isPause = true;
}

void ArkEngine::ArkDX11::FBXAnimator::RestartFBXAnimation()
{
	_isPause = false;
}

void ArkEngine::ArkDX11::FBXAnimator::ReadAnimData(std::wstring fileName)
{
	std::wstring fullPath = _animPath + fileName + L".clip";

	if (ResourceManager::GetInstance()->GetAnimationData(fullPath) == nullptr)
	{
		std::unique_ptr<ArkEngine::Utils> utils = std::make_unique<Utils>();

		std::unique_ptr<FileUtils> file = std::make_unique<FileUtils>();
		file->Open(fullPath, FileMode::Read);

		AnimData* animationData;

		animationData = new AnimData();

		animationData->anim = std::make_shared<ModelAnimation>();
		animationData->anim->name = file->Read<std::string>();
		animationData->anim->duration = file->Read<float>();
		animationData->anim->frameRate = file->Read<float>();
		animationData->anim->frameCount = file->Read<unsigned int>();

		unsigned int keyframesCount = file->Read<unsigned int>();


		for (unsigned int i = 0; i < keyframesCount; i++)
		{
			animationData->keyFrame = std::make_shared<ModelKeyframe>();
			animationData->keyFrame->boneName = utils->ToWString(file->Read<std::string>());

			unsigned int size = file->Read<unsigned int>();

			if (size > 0)
			{
				animationData->keyFrame->transforms.resize(size);
				void* ptr = &animationData->keyFrame->transforms[0];
				file->Read(&ptr, sizeof(ModelKeyframeData) * size);
			}

			animationData->anim->keyframes[animationData->keyFrame->boneName] = animationData->keyFrame;
		}

		ResourceManager::GetInstance()->AddAnimationData(fullPath, animationData);
	}

	_anim = ResourceManager::GetInstance()->GetAnimationData(fullPath)->anim;
	_frame = ResourceManager::GetInstance()->GetAnimationData(fullPath)->keyFrame;

	_animationIndex.push_back(_anim);
}

void ArkEngine::ArkDX11::FBXAnimator::UpdateAnimTransforms(float speed, float deltaTime, unsigned int animIndex)
{
	//_isPlaying = true;

	if (_isPause == true)
	{
		speed = 0;
		deltaTime = 0;
	}
	else
	{
		_frameCount += speed * deltaTime;
		_frameCount = min(_frameCount, _anim->duration);
	}

	// ���� �����Ӱ� ���� ������ �ε��� ���
	unsigned int prevFrameIndex = static_cast<unsigned int>(_frameCount);
	unsigned int nextFrameIndex = prevFrameIndex + 1;

	// �ִϸ��̼� ������ ��
	unsigned int totalFrames = _anim->frameCount;

	// ������ ���� ���� ���(0.0 ���� 1.0 ���� ��)
	float interfolationFactor = _frameCount - static_cast<float>(prevFrameIndex);

	// ���� �����Ӱ� ���� �������� �����Ͽ� �ִϸ��̼� ������Ʈ
	// ���� �������� ������ �������� ���, ���� �������� ù��° ���������� ����
	unsigned int nextFrame = (nextFrameIndex >= totalFrames) ? 0 : nextFrameIndex;

	// Ư�� �ִϸ��̼� �ε����� ���� ���� ��ȯ ����� ���
	std::vector<Matrix> tempAnimBoneTransforms(250, Matrix::Identity); // �ӽ� �ִϸ��̼� ���� ��� �迭

	/// �ִϸ��̼� �����Ӻ��� ���� ��ȯ ��� ���
	// ��
	for (unsigned int b = 0; b < GetBoneCount(); b++)
	{
		auto modelBone = GetBonesByIndex(b);

		/// ���ʺ��� ������ ���� �������°� �س��� �Ѵ�
		// �߿�κ�
		// T����
		// toRootMatrix : ���� ������ ��Ʈ �������� ��ȯ���
		// boneOffsetInverse
		Matrix toRootMatrix = modelBone->transform;


		// invGloval : ��Ʈ ������ ����������� �����
		// Gloval -> Relative
		// offsetTM
		_invGlobal = toRootMatrix.Invert();

		// ���� ���� �θ� ���� ���� �ε���
		int parentIndex = modelBone->parentIndex;

		// Animation
		// matParent : �θ� ���� ��ȯ ���
		Matrix matParent = Matrix::Identity;
		if (parentIndex >= 0)
			matParent = tempAnimBoneTransforms[parentIndex];

		Matrix matAnimation;

		_frame = _anim->GetKeyframe(modelBone->name);

		if (_frame != nullptr)
		{
			ModelKeyframeData prevFrameData = _frame->transforms[prevFrameIndex];
			ModelKeyframeData nextFrameData = _frame->transforms[nextFrame];

			/// �Ű������� �ϳ��� �޾Ƽ� �غ��� ������
			//if (changeAnimIndex != -1)
			//{
			//	nextFrameData = 
			//}

			// Ư�� ���� Ư�� �����ӿ� ���� SRT�� ������
			// ���� �����Ӱ� ���� �������� ��ȯ ��� ��������
			//_nextAnimFrameData = prevFrameData;

			Vector3 interpolatedScale = Vector3::Lerp(prevFrameData.scale, nextFrameData.scale, interfolationFactor);
			Quaternion interpolatedRotation = Quaternion::Slerp(prevFrameData.rotation, nextFrameData.rotation, interfolationFactor);
			Vector3 interpolatedTranslation = Vector3::Lerp(prevFrameData.translation, nextFrameData.translation, interfolationFactor);

			matAnimation = Matrix::CreateScale(interpolatedScale) * Matrix::CreateFromQuaternion(interpolatedRotation) * Matrix::CreateTranslation(interpolatedTranslation);
		}
		else
		{
			// �����ӿ� �ش��ϴ� Ű�������� ������ ���� ��� ���
			matAnimation = Matrix::Identity;
		}

		// matAnimation : ���� �����ӿ����� �ִϸ��̼ǿ� ���� ��ȯ ���
		// tempAnimBoneTransforms : ���� ���� ���� �ִϸ��̼��� ������ ��ȯ ����� ����Ͽ� ����
		// ���뿡�� �۷ι��� ������ ��ȯ���
		tempAnimBoneTransforms[b] = matAnimation * matParent;

		//std::vector<Matrix> changeAnim = Matrix::Lerp(_idleAnimBoneTransforms, tempAnimBoneTransforms, _transitionFactor);

		// ���
		// �� �ִϸ��̼� ��� �迭�� ���� ���� ���� ���� ��ȯ ����� ����
		// ���������δ� ��Ʈ ������ ���� �������� ���� ��ȯ ����� _animTransforms�� �����Ѵ�
		DirectX::XMMATRIX finalBone = DirectX::XMMatrixMultiply(_invGlobal, tempAnimBoneTransforms[b]);



		// ���� ũ�⸦ ���� Ȯ���ϰ� ������ ��������
		DirectX::XMFLOAT4X4 boneTM;
		DirectX::XMStoreFloat4x4(&boneTM, finalBone);

		if (_boneTransformMatrix.size() <= b)
		{
			_boneTransformMatrix.resize(GetBoneCount());
		}

		_boneTransformMatrix[b] = boneTM;

	}
}


void ArkEngine::ArkDX11::FBXAnimator::UpdateAnimTransforms(float speed, float deltaTime, std::string animName)
{
	//_isPlaying = true;

	if (_isPause == true)
	{
		speed = 0;
		deltaTime = 0;
	}
	else
	{
		_frameCount += speed * deltaTime;
		_frameCount = min(_frameCount, _anim->duration);
	}

	// ���� �����Ӱ� ���� ������ �ε��� ���
	unsigned int prevFrameIndex = static_cast<unsigned int>(_frameCount);
	unsigned int nextFrameIndex = prevFrameIndex + 1;

	// �ִϸ��̼� ������ ��
	unsigned int totalFrames = _anim->frameCount;

	// ������ ���� ���� ���(0.0 ���� 1.0 ���� ��)
	float interfolationFactor = _frameCount - static_cast<float>(prevFrameIndex);

	// ���� �����Ӱ� ���� �������� �����Ͽ� �ִϸ��̼� ������Ʈ
	// ���� �������� ������ �������� ���, ���� �������� ù��° ���������� ����
	unsigned int nextFrame = (nextFrameIndex >= totalFrames) ? 0 : nextFrameIndex;

	// Ư�� �ִϸ��̼� �ε����� ���� ���� ��ȯ ����� ���
	std::vector<Matrix> tempAnimBoneTransforms(250, Matrix::Identity); // �ӽ� �ִϸ��̼� ���� ��� �迭

	/// �ִϸ��̼� �����Ӻ��� ���� ��ȯ ��� ���
	// ��
	for (unsigned int b = 0; b < GetBoneCount(); b++)
	{
		auto modelBone = GetBonesByIndex(b);

		/// ���ʺ��� ������ ���� �������°� �س��� �Ѵ�
		// �߿�κ�
		// T����
		// toRootMatrix : ���� ������ ��Ʈ �������� ��ȯ���
		// boneOffsetInverse
		Matrix toRootMatrix = modelBone->transform;


		// invGloval : ��Ʈ ������ ����������� �����
		// Gloval -> Relative
		// offsetTM
		_invGlobal = toRootMatrix.Invert();

		// ���� ���� �θ� ���� ���� �ε���
		int parentIndex = modelBone->parentIndex;

		// Animation
		// matParent : �θ� ���� ��ȯ ���
		Matrix matParent = Matrix::Identity;
		if (parentIndex >= 0)
			matParent = tempAnimBoneTransforms[parentIndex];

		Matrix matAnimation;

		_frame = _anim->GetKeyframe(modelBone->name);

		if (_frame != nullptr)
		{
			ModelKeyframeData prevFrameData = _frame->transforms[prevFrameIndex];
			ModelKeyframeData nextFrameData = _frame->transforms[nextFrame];

			/// �Ű������� �ϳ��� �޾Ƽ� �غ��� ������
			//if (changeAnimIndex != -1)
			//{
			//	nextFrameData = 
			//}

			// Ư�� ���� Ư�� �����ӿ� ���� SRT�� ������
			// ���� �����Ӱ� ���� �������� ��ȯ ��� ��������
			//_nextAnimFrameData = prevFrameData;

			Vector3 interpolatedScale = Vector3::Lerp(prevFrameData.scale, nextFrameData.scale, interfolationFactor);
			Quaternion interpolatedRotation = Quaternion::Slerp(prevFrameData.rotation, nextFrameData.rotation, interfolationFactor);
			Vector3 interpolatedTranslation = Vector3::Lerp(prevFrameData.translation, nextFrameData.translation, interfolationFactor);

			matAnimation = Matrix::CreateScale(interpolatedScale) * Matrix::CreateFromQuaternion(interpolatedRotation) * Matrix::CreateTranslation(interpolatedTranslation);
		}
		else
		{
			// �����ӿ� �ش��ϴ� Ű�������� ������ ���� ��� ���
			matAnimation = Matrix::Identity;
		}

		// matAnimation : ���� �����ӿ����� �ִϸ��̼ǿ� ���� ��ȯ ���
		// tempAnimBoneTransforms : ���� ���� ���� �ִϸ��̼��� ������ ��ȯ ����� ����Ͽ� ����
		// ���뿡�� �۷ι��� ������ ��ȯ���
		tempAnimBoneTransforms[b] = matAnimation * matParent;

		//std::vector<Matrix> changeAnim = Matrix::Lerp(_idleAnimBoneTransforms, tempAnimBoneTransforms, _transitionFactor);

		// ���
		// �� �ִϸ��̼� ��� �迭�� ���� ���� ���� ���� ��ȯ ����� ����
		// ���������δ� ��Ʈ ������ ���� �������� ���� ��ȯ ����� _animTransforms�� �����Ѵ�
		DirectX::XMMATRIX finalBone = DirectX::XMMatrixMultiply(_invGlobal, tempAnimBoneTransforms[b]);



		// ���� ũ�⸦ ���� Ȯ���ϰ� ������ ��������
		DirectX::XMFLOAT4X4 boneTM;
		DirectX::XMStoreFloat4x4(&boneTM, finalBone);

		if (_boneTransformMatrix.size() <= b)
		{
			_boneTransformMatrix.resize(GetBoneCount());
		}

		_boneTransformMatrix[b] = boneTM;

	}
}

Vector3 ArkEngine::ArkDX11::FBXAnimator::ExtractScale(const Matrix& matrix)
{
	return Vector3(matrix._11, matrix._22, matrix._33);
}

Quaternion ArkEngine::ArkDX11::FBXAnimator::ExtractRotation(const Matrix& matrix)
{
	float trace = matrix._11 + matrix._22 + matrix._33;

	if (trace > 0.0f)
	{
		float s = 0.5f / sqrtf(trace + 1.0f);
		return Quaternion(
			(matrix._23 - matrix._32) * s,
			(matrix._31 - matrix._13) * s,
			(matrix._12 - matrix._21) * s,
			0.25f / s
		);
	}
	else
	{
		if (matrix._11 > matrix._22 && matrix._11 > matrix._33)
		{
			float s = 2.0f * sqrtf(1.0f + matrix._11 - matrix._22 - matrix._33);
			return Quaternion(
				0.25f * s,
				(matrix._12 + matrix._21) / s,
				(matrix._13 + matrix._31) / s,
				(matrix._23 - matrix._32) / s
			);
		}
		else if (matrix._22 > matrix._33)
		{
			float s = 2.0f * sqrtf(1.0f + matrix._22 - matrix._11 - matrix._33);
			return Quaternion(
				(matrix._12 + matrix._21) / s,
				0.25f * s,
				(matrix._23 + matrix._32) / s,
				(matrix._31 - matrix._13) / s
			);
		}
		else
		{
			float s = 2.0f * sqrtf(1.0f + matrix._33 - matrix._11 - matrix._22);
			return Quaternion(
				(matrix._13 + matrix._31) / s,
				(matrix._23 + matrix._32) / s,
				0.25f * s,
				(matrix._12 - matrix._21) / s
			);
		}
	}
}

Vector3 ArkEngine::ArkDX11::FBXAnimator::ExtractTranslation(const Matrix& matrix)
{
	return Vector3(matrix._41, matrix._42, matrix._43);
}

std::shared_ptr<ModelAnimation> ArkEngine::ArkDX11::FBXAnimator::GetAnimationByIndex(unsigned int index)
{
	// �ε����� ��ȿ�� ���� ���� �ִ��� Ȯ���մϴ�.
	// ���� �ε����� ��ȿ���� ������ nullptr�� ��ȯ�մϴ�.
	return (index < 0 || index >= _animationIndex.size()) ? nullptr : _animationIndex[index];
}

std::shared_ptr<ModelAnimation> ArkEngine::ArkDX11::FBXAnimator::GetAnimationByName(const std::string& animName)
{
	for (auto index : _animationIndex)
	{
		if (index->name == animName)
		{
			return index;
		}
	}

	return nullptr;
}

std::shared_ptr<ModelBone> ArkEngine::ArkDX11::FBXAnimator::GetBonesByIndex(unsigned int index)
{
	return (index < 0 || index >= _animBones.size() ? nullptr : _animBones[index]);
}

std::shared_ptr<ModelBone> ArkEngine::ArkDX11::FBXAnimator::GetBonesByName(const std::string& boneName)
{
	auto name = std::wstring(boneName.begin(), boneName.end());

	for (auto index : _animBones)
	{
		if (index->name == name)
		{
			return index;
		}
	}
	
	return nullptr;
}

unsigned int ArkEngine::ArkDX11::FBXAnimator::GetBoneCount()
{
	return _animBones.size();
}


Matrix ArkEngine::ArkDX11::FBXAnimator::Lerp(const Matrix& start, const Matrix& end, float t)
{
	return Matrix::Lerp(start, end, t);
}