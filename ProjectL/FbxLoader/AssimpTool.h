/// <summary>
/// 
/// FBX 파일을 mesh, clip 파일로 정제하기 위한 클래스
/// 
/// 김현재 
/// 
/// FBX 파일을 정제한 파일이 Bin에 존재하지않는다면 정제하고
/// 이미 존재한다면 정제한 목록만 넘기도록 함
/// 
/// 윤종화
/// </summary>

#pragma once
#include <string>

namespace ArkEngine
{
	class FBXConverter;
}

namespace ArkEngine
{
	namespace FBXLoader
	{
		class __declspec(dllexport) AssimpTool
		{
		public:
			AssimpTool();
			~AssimpTool();

		public:
			std::vector<std::string> GetSkinnedMeshName();

			std::vector<std::string> GetAnimationName();

		private:
			void Initialize();

		private:
			void InitOnlyName();

		private:
			std::vector<std::string> FindAllAssets(const std::string& folderName, const std::string& extension);

		private:
			void LoadSkinnedMesh();
			void LoadAnimationMesh();

		private:
			void FindNewMesh();

		private:
			std::vector<std::string> _skinMeshNameList;
			std::vector<std::string> _animationNameList;
		};
	}
}