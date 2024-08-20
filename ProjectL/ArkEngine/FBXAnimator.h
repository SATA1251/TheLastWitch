/// FBX Animator Ŭ����
/// ���ӿ����� Animator ���۳�Ʈ ����� ���� ��
/// 
/// ������

#define MAX_MODEL_TRANSFORMS 150
#define MAX_MODEL_KEYFRAMES 500

#pragma once
#include <string>
#include <array>
#include <filesystem>
#include "ModelAnimation.h"

struct AnimTransform
{
	// 250���� ���������� ���� �� �ִ�
	using TransformArrayType = std::array<Matrix, MAX_MODEL_TRANSFORMS>;

	// 2�� �迭
	std::array<TransformArrayType, MAX_MODEL_KEYFRAMES> _transforms;
};

struct AnimData
{
	std::shared_ptr<ModelAnimation> anim;
	std::shared_ptr<ModelKeyframe> keyFrame;
};

struct ModelMesh;
struct ModelAnimation;
struct ModelKeyframe;
struct ModelBone;
struct ModelKeyframeData;

namespace ArkEngine
{
	namespace ArkDX11
	{
		class FBXAnimator
		{
		public:
			FBXAnimator();
			~FBXAnimator();

		private:
			// �ִϸ��̼��� �а� ���� �����Ű�� �Լ���
			void ReadAnimData(std::wstring fileName);
			void UpdateAnimTransforms(float speed, float deltaTime, unsigned int animIndex);
			void UpdateAnimTransforms(float speed, float deltaTime, std::string animName);

		public:
			// FBXMesh Ŭ�������� ����ϴ� �Լ���
			bool CheckClipFile(std::string fileName);
			bool PlayAnimationOnce(float speed, float deltaTime, int animIndex);
			bool PlayAnimationOnce(float speed, float deltaTime, std::string animName);
			bool PlayAnimationContinuous(float speed, float deltaTime, int animIndex);
			bool PlayAnimationContinuous(float speed, float deltaTime, std::string animName);
			void StopFBXAnimation();
			void PauseFBXAnimation();
			void RestartFBXAnimation();
			const std::vector<std::string>& GetClipNames();
			void SetCurrentFrame(int frame);
			float GetCurrentFrame();
			float GetMaxFrame();
			const std::string& GetAnimationName();
			const int GetIndexOfBone(const std::string& boneName);
			const DirectX::XMFLOAT4X4 GetBoneTransform(const std::string& boneName);
			const DirectX::XMFLOAT4X4 GetBoneTransform(int boneIndex);
			const DirectX::XMFLOAT4X4 GetBoneAnimation(int boneIndex);


		private:
			// BoneTransform�� �ʿ��� �Լ���
			std::shared_ptr<ModelBone> GetBonesByIndex(unsigned int index);
			std::shared_ptr<ModelBone> GetBonesByName(const std::string& name);
			std::shared_ptr<ModelAnimation> GetAnimationByIndex(unsigned int index);
			std::shared_ptr<ModelAnimation> GetAnimationByName(const std::string& animName);
			unsigned int GetBoneCount();

			// Matrix ���� �Լ���
			Matrix Lerp(const Matrix& start, const Matrix& end, float t);
			Vector3 ExtractScale(const Matrix& matrix);
			Quaternion ExtractRotation(const Matrix& matrix);
			Vector3 ExtractTranslation(const Matrix& matrix);

		private:
			std::vector<AnimTransform> _animTransforms;
			std::shared_ptr<ModelKeyframe> _frame;

		private:
			Matrix _invGlobal;
			std::vector<std::shared_ptr<ModelAnimation>> _animationIndex;
			std::wstring _animPath;
			bool _isPause;

			// FBXMesh�� �������ִ� ������
			float _frameCount;

		public:
			std::shared_ptr<ModelAnimation> _anim;
			std::vector<std::shared_ptr<ModelBone>> _animBones;
			std::vector<DirectX::XMFLOAT4X4> _boneTransformMatrix;

			std::vector<std::string> _animationClips;
		};
	}
}