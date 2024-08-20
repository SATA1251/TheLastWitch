#include <iostream>
#include <filesystem>
#include <algorithm>
#include "AssimpTool.h"
#include "FBXConverter.h"

ArkEngine::FBXLoader::AssimpTool::AssimpTool()
	: _skinMeshNameList(), _animationNameList()
{
	auto parsingList = FindAllAssets("Resources/Models", "mesh");
	auto aseetList = FindAllAssets("Resources/Assets", "fbx");
	
	//Initialize();

	if (parsingList.size() != aseetList.size())
	{
		Initialize();
	}
	else
	{
		InitOnlyName();
	}
}

ArkEngine::FBXLoader::AssimpTool::~AssimpTool()
{
}

void ArkEngine::FBXLoader::AssimpTool::Initialize()
{
	LoadSkinnedMesh();

	LoadAnimationMesh();
}

void ArkEngine::FBXLoader::AssimpTool::InitOnlyName()
{
	auto skinMeshList = FindAllAssets("Resources/Assets", "fbx");

	/// Skinned Mesh
		// FBX -> Memory
	for (int i = 0; i < skinMeshList.size(); i++)
	{
		std::string fileName = "";
		std::string nowString = skinMeshList[i];

		size_t lastSlashPos = nowString.find_last_of('/');

		if (lastSlashPos != std::string::npos)
		{
			size_t dotFbxPos = nowString.find(".fbx");
			if (dotFbxPos != std::string::npos)
			{
				fileName = nowString.substr(lastSlashPos + 1, dotFbxPos - lastSlashPos - 1);
			}
		}

		std::string filePath = fileName + "/" + fileName;

		_skinMeshNameList.emplace_back(filePath);
	}

	auto animMeshList = FindAllAssets("Resources/AnimAssets", "fbx");

	for (int i = 0; i < animMeshList.size(); i++)
	{
		std::string animPath = animMeshList[i];
		std::wstring wideAnimPath = std::wstring(animPath.begin(), animPath.end());

		size_t fbxPos = animPath.find(".fbx");
		if (fbxPos != std::string::npos)
		{
			size_t lastSlashPos = animPath.rfind('/', fbxPos);

			if (lastSlashPos != std::string::npos)
			{
				std::string extractedString1 = animPath.substr(lastSlashPos, fbxPos - lastSlashPos);

				size_t namePos = animPath.find(extractedString1);
				size_t lastNamePos = animPath.rfind('/', namePos - 1);

				if (lastNamePos != std::string::npos)
				{
					std::string resultString = animPath.substr(lastNamePos + 1);

					size_t finalPos = resultString.find(".fbx");

					if (finalPos != std::string::npos)
					{
						resultString.replace(finalPos, 4, "");

						_animationNameList.emplace_back(resultString);
					}
				}
			}
		}
	}
}

std::vector<std::string> ArkEngine::FBXLoader::AssimpTool::GetSkinnedMeshName()
{
	FindNewMesh();
	return _skinMeshNameList;
}

std::vector<std::string> ArkEngine::FBXLoader::AssimpTool::GetAnimationName()
{
	return _animationNameList;
}

void ArkEngine::FBXLoader::AssimpTool::LoadSkinnedMesh()
{
	/// Skinned Mesh
	auto skinMeshList = FindAllAssets("Resources/Assets", "fbx");

		// FBX -> Memory
	for (int i = 0; i < skinMeshList.size(); i++)
	{
		std::unique_ptr<ArkEngine::FBXConverter> converter = std::make_unique<ArkEngine::FBXConverter>();

		std::string fileName = "";
		std::string nowString = skinMeshList[i];

		size_t lastSlashPos = nowString.find_last_of('/');

		if (lastSlashPos != std::string::npos)
		{
			size_t dotFbxPos = nowString.find(".fbx");
			if (dotFbxPos != std::string::npos)
			{
				fileName = nowString.substr(lastSlashPos + 1, dotFbxPos - lastSlashPos - 1);
			}
		}

		std::string filePath = fileName + "/" + fileName;

		std::wstring wideAssetString = std::wstring(skinMeshList[i].begin(), skinMeshList[i].end()); // std::string을 std::wstring으로 변환
		std::wstring wideFileString = std::wstring(filePath.begin(), filePath.end()); // std::string을 std::wstring으로 변환

		// Memory -> CustomData
		converter->ReadAssetFile(wideAssetString);
		converter->ExportMaterialData(wideFileString);
		converter->ExportModelData(wideFileString);

		_skinMeshNameList.emplace_back(filePath);
	}
}

void ArkEngine::FBXLoader::AssimpTool::LoadAnimationMesh()
{
	auto animMeshList = FindAllAssets("Resources/AnimAssets", "fbx");

	for (int i = 0; i < animMeshList.size(); i++)
	{
		std::unique_ptr<ArkEngine::FBXConverter> converter = std::make_unique<ArkEngine::FBXConverter>();

		std::string animPath = animMeshList[i];
		std::wstring wideAnimPath = std::wstring(animPath.begin(), animPath.end());

		converter->ReadAnimAssetFile(wideAnimPath);

		size_t fbxPos = animPath.find(".fbx");
		if (fbxPos != std::string::npos)
		{
			size_t lastSlashPos = animPath.rfind('/', fbxPos);

			if (lastSlashPos != std::string::npos)
			{
				std::string extractedString1 = animPath.substr(lastSlashPos, fbxPos - lastSlashPos);

				size_t namePos = animPath.find(extractedString1);
				size_t lastNamePos = animPath.rfind('/', namePos - 1);

				if (lastNamePos != std::string::npos)
				{
					std::string resultString = animPath.substr(lastNamePos + 1);

					size_t finalPos = resultString.find(".fbx");

					if (finalPos != std::string::npos)
					{
						resultString.replace(finalPos, 4, "");
						std::wstring wideExportPath = std::wstring(resultString.begin(), resultString.end());
						converter->ExportAnimationData(wideExportPath);

						_animationNameList.emplace_back(resultString);
					}
				}
			}
		}
	}
}

void ArkEngine::FBXLoader::AssimpTool::FindNewMesh()
{
	auto assetList = FindAllAssets("Resources/Assets", "fbx");
	auto parsingList = FindAllAssets("Resources/Models", "mesh");

	if (assetList.size() != parsingList.size())
	{
		for (int i = 0; i < assetList.size(); i++)
		{
			std::string fileName = "";
			std::string nowString = assetList[i];

			size_t lastSlashPos = nowString.find_last_of('/');

			if (lastSlashPos != std::string::npos)
			{
				size_t dotFbxPos = nowString.find(".fbx");
				if (dotFbxPos != std::string::npos)
				{
					fileName = nowString.substr(lastSlashPos + 1, dotFbxPos - lastSlashPos - 1);
				}
			}

			for (int j = 0; j < parsingList.size(); j++)
			{
				if (parsingList[j].find(fileName) != std::string::npos)
				{
					break;
				}
				else if (j == parsingList.size() - 1)
				{
					std::string filePath = fileName + "/" + fileName;

					std::wstring wideAssetString = std::wstring(assetList[i].begin(), assetList[i].end()); // std::string을 std::wstring으로 변환
					std::wstring wideFileString = std::wstring(filePath.begin(), filePath.end()); // std::string을 std::wstring으로 변환

					std::unique_ptr<ArkEngine::FBXConverter> converter = std::make_unique<ArkEngine::FBXConverter>();

					// Memory -> CustomData
					converter->ReadAssetFile(wideAssetString);
					converter->ExportMaterialData(wideFileString);
					converter->ExportModelData(wideFileString);

					_skinMeshNameList.emplace_back(filePath);
				}
			}
		}
	}
}

std::vector<std::string> ArkEngine::FBXLoader::AssimpTool::FindAllAssets(const std::string& folderName, const std::string& extension)
{
	std::string folderPath = folderName + "/";
	std::string targetExtension = "." + extension;
	std::vector<std::string> filenameList;

	if (!std::filesystem::exists(folderPath) || !std::filesystem::is_directory(folderPath)) 
	{
		return filenameList;
	}

	for (const auto& entry : std::filesystem::directory_iterator(folderPath))
	{
		if (entry.is_regular_file() && entry.path().extension() == targetExtension)
		{
			filenameList.emplace_back(folderPath + entry.path().filename().string());
		}
		else if (entry.is_directory())
		{
			std::vector<std::string> subfolderFiles = FindAllAssets(entry.path().string(), extension);

			filenameList.insert(filenameList.end(), subfolderFiles.begin(), subfolderFiles.end());
		}
	}

	std::sort(filenameList.begin(), filenameList.end());

	return filenameList;
}