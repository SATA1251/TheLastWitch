/// <summary>
/// 
/// 메쉬의 애니메이션 처리를 위한
/// 애니메이터 클래스
/// 
/// 윤종화
/// </summary>
#pragma once
#include <unordered_map>


class ASEParser;


struct Frame 
{
	unsigned int nowFrame;
	unsigned int nextFrame;
	unsigned int nextFrameIndex;
};


namespace ArkEngine
{
	namespace ArkDX11
	{

		class Animator
		{
		public:
			Animator(ASEParser* parser, int meshCount);

			~Animator();

		public:
			void PlayAnimationOnce(float deltaTime);
			void PlayAnimationContinious(float deltaTime);

			void StopAseAnimation();

		private:
			void InitAseAnimation();
			void AseAnimation(float deltaTime);

		private:
			ASEParser* _aseParser;

			std::unordered_map<unsigned int, Frame> _posFrame_Mesh;
			std::unordered_map<unsigned int, Frame> _rotFrame_Mesh;

		private:
			int _meshCount;

			float _timer;



		};
	}
}
