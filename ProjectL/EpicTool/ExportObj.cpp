#include "ExportObj.h"

#include "KunrealAPI.h"
#include <filesystem>
#include <fstream>

EpicTool::ExportObj::ExportObj()
{

}

EpicTool::ExportObj::~ExportObj()
{

}

void EpicTool::ExportObj::Initialize()
{

}

void EpicTool::ExportObj::ExportToObj(const std::string& fileName)
{
	std::vector<std::vector<std::vector<DirectX::XMFLOAT3>>> vertexObjectList;

	std::vector<std::vector<std::vector<unsigned int>>> objectIndexList;

	std::string folderPath = std::filesystem::current_path().string();

	std::string filePath = folderPath + "/Resources/MapObj/" + fileName + ".obj";
	
	std::ofstream file(filePath);

	int offset = 1;

	vertexObjectList = GRAPHICS->GetAllMeshVertex();

	objectIndexList = GRAPHICS->GetAllMeshIndex();

	for (int i = 0; i < vertexObjectList.size(); i++)
	{
		for (int j = 0; j < vertexObjectList[i].size(); j++)
		{
			for (int w = 0; w < vertexObjectList[i][j].size(); w++)
			{
				file << "v " << vertexObjectList[i][j][w].x << " " << vertexObjectList[i][j][w].y << " " << vertexObjectList[i][j][w].z << std::endl;
			}
		}
	}

	for (int i = 0; i < objectIndexList.size(); i++)
	{
		for (int j = 0; j < objectIndexList[i].size(); j++)
		{
			file << "o Object" << i + 1 << std::endl; // Object 이름 설정

			for (size_t w = 0; w < objectIndexList[i][j].size(); w += 3)
			{
				file << "f ";
				file << objectIndexList[i][j][w] + offset << " ";
				file << objectIndexList[i][j][w + 1] + offset << " ";
				file << objectIndexList[i][j][w + 2] + offset << std::endl;
			}

			offset += vertexObjectList[i][j].size(); // 정점의 수만큼 offset 증가
		}
	}

	file.close();
}
