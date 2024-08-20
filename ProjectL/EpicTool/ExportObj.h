#pragma once
#include <vector>
#include <string>

namespace KunrealEngine
{
	
}

namespace EpicTool
{
	class ExportObj
	{
	public:
		ExportObj();
		~ExportObj();

	public:
		void Initialize();
		void ExportToObj(const std::string& fileName);



	private:
		//std::vector<unsigned int> _totalMeshVertex;  // 버텍스 벡터
		//std::vector<std::vector<unsigned int>> _totalMesh; // 버텍스를 들고있는 오브젝트의 메쉬들
		////std::vector<std::vector<std::vector<DirectX::XMFLOAT3>>> _totalMeshList; // 버텍스를 들고있는 오브젝트들
	};
}

