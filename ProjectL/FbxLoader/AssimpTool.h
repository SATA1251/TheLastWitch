/// <summary>
/// 
/// FBX ������ mesh, clip ���Ϸ� �����ϱ� ���� Ŭ����
/// 
/// ������ 
/// 
/// FBX ������ ������ ������ Bin�� ���������ʴ´ٸ� �����ϰ�
/// �̹� �����Ѵٸ� ������ ��ϸ� �ѱ⵵�� ��
/// 
/// ����ȭ
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