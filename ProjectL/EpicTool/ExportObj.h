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
		//std::vector<unsigned int> _totalMeshVertex;  // ���ؽ� ����
		//std::vector<std::vector<unsigned int>> _totalMesh; // ���ؽ��� ����ִ� ������Ʈ�� �޽���
		////std::vector<std::vector<std::vector<DirectX::XMFLOAT3>>> _totalMeshList; // ���ؽ��� ����ִ� ������Ʈ��
	};
}

