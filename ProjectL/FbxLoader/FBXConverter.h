/// FBXLoader
/// FBX 파일을 받아와서 가공한후 그래픽스 엔진에 전달해주는 역할을 한다
/// 김현재
#pragma once
#include <vector>
#include <memory>
#include <map>

namespace ArkEngine
{
	class Utils;
	class FileUtils;

	struct asBone;
	struct asMesh;
	struct asMaterial;

	// Animation
	struct asAnimation;
	struct asAnimationNode;
	struct asKeyFrameData;
}

namespace Assimp
{
	class Importer;
}

struct aiMaterial;
struct aiScene;
struct aiNode;
struct aiAnimation;
struct aiNodeAnim;
struct aiBone;

namespace ArkEngine
{
	class __declspec(dllexport) FBXConverter
	{
	public:

		FBXConverter();
		~FBXConverter();

	public:
		void ReadAssetFile(std::wstring file);
		void ReadAnimAssetFile(std::wstring filePath);
		void ExportModelData(std::wstring savePath);
		void ExportMaterialData(std::wstring savePath);
		void ExportAnimationData(std::wstring savePath, unsigned int index = 0);
	private:
		// 나중에 int32로 바꿔보자(구지?)
		void ReadModelData(aiNode* node, int index, int parent);
		void ReadMeshData(aiNode* node, int bone);
		void ReadSkinData();
		void WriteModelFile(std::wstring finalPath);

	private:
		void ReadMaterialData();
		void WriteMaterialData(std::wstring finalPath);
		std::string WriteTexture(std::string saveFolder, std::string file);
		
		// Animation
	private:
		std::shared_ptr<asAnimation> ReadAnimationData(aiAnimation* srcAnimation, std::string animName);
		
		// SRT와 관련된 부분을 추출하는 함수
		std::shared_ptr<asAnimationNode> ParseAnimationNode(std::shared_ptr<asAnimation> animation, aiNodeAnim* srcNode);
		void ReadKeyframeData(std::shared_ptr<asAnimation> animation, aiNode* node, std::map<std::string, std::shared_ptr<asAnimationNode>>& cache);
		void WriteAnimationData(std::shared_ptr<asAnimation> animation, std::wstring finalPath);
	private:
		unsigned int GetBoneIndex(const std::string& name);


	private:
		std::shared_ptr<Assimp::Importer> _importer;
		const aiScene* _scene;

	private:
		std::vector<std::shared_ptr<asBone>> _bones;
		std::vector<std::shared_ptr<asMesh>> _meshes;
		std::vector<std::shared_ptr<asMaterial>> _materials;

		std::wstring _assetPath;
		std::wstring _modelPath;
		std::wstring _texturePath;

		aiMaterial* _material;
		std::shared_ptr<asBone> _bone;
		aiBone* srcMeshBone;

		Utils* _utils;
	};
}
