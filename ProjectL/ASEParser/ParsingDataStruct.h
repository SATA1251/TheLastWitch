#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <DirectXMath.h>
#include "DXTK/SimpleMath.h"

namespace ASE_Parser
{
	struct BoneWeight
	{
		BoneWeight(unsigned int index, float weight);

		unsigned int m_BoneIndex;
		float m_Weight;
	};

	struct Vertex
	{
		Vertex();
		~Vertex();

		DirectX::SimpleMath::Vector3 m_Position;
		DirectX::SimpleMath::Vector3 m_Normal;
		DirectX::SimpleMath::Vector3 m_Texture;

		bool isVertexSet;	// 버텍스 정보들이 채워졌는가
		std::vector<unsigned int> m_splitList;

		std::vector<BoneWeight> m_BoneWeightList;

		// DirectX::SimpleMath::Vector2 m_Color;
		// color 값도 잘 안쓰일것 같은데 굳이 갖고있으면 메모리 낭비이므로 일단 주석처리
	};

	struct Face
	{
		Face();

		std::vector<unsigned int> m_indexList;

		DirectX::SimpleMath::Vector3 m_Normal;
	};

	struct Tvert
	{
		Tvert();

		// unsigned int m_TvertNum;
		// face의 수와 같기때문에 굳이 가지고 있을 필요가 없을거같아 주석처리

		DirectX::SimpleMath::Vector3 m_UVW;
	};

	struct Tface
	{
		Tface();

		// unsigned int m_TfaceNum;
		// face의 수와 같기때문에 굳이 가지고 있을 필요가 없을거같아 주석처리

		std::vector<unsigned int> m_indexList;
	};

	// 메쉬, 헬퍼, 쉐이프 오브젝트를 묶기 위함 -> 노드 관계를 위해 부모와 자식의 포인터를 부모를 담는 백터로 갖고 있기 위해
	struct ASEOBJECT
	{
		ASEOBJECT();
		~ASEOBJECT();

		std::string m_NodeName;
		std::string m_NodeParent;

		DirectX::SimpleMath::Vector3 m_Tm_Row0;
		DirectX::SimpleMath::Vector3 m_Tm_Row1;
		DirectX::SimpleMath::Vector3 m_Tm_Row2;
		DirectX::SimpleMath::Vector3 m_Tm_Row3;

		DirectX::SimpleMath::Vector3 m_Tm_Pos;

		DirectX::SimpleMath::Vector3 m_Tm_RotAxis;
		float m_Tm_RotAngle;

		DirectX::SimpleMath::Vector3 m_Tm_Scale;
		DirectX::SimpleMath::Vector3 m_Tm_ScaleAxis;
		float m_Tm_ScaleAxisAng;


		std::unordered_map<int, DirectX::SimpleMath::Vector3> m_Animation_Pos;
		std::unordered_map<int, DirectX::SimpleMath::Vector4> m_Animation_Rot;

		std::vector<int> m_Animation_Pos_Index;
		std::vector<int> m_Animation_Rot_Index;
		int m_nextFrame_POS_Index;
		int m_nextFrame_Rot_Index;

		std::vector<ASEOBJECT*> _parentObject;
		std::vector<ASEOBJECT*> _childObject;

		DirectX::XMFLOAT4X4 _originWorldTM;	// TM_ROW를 조합한 월드 행렬
		DirectX::XMFLOAT4X4 _originLocalTM;	// 애니메이션 초기화를 위해 가지고 있을 기본 로컬 행렬

		std::unordered_map<unsigned int, DirectX::XMFLOAT4X4> _localTM;			// 월드 행렬을 통해 계산한 로컬 행렬
		std::unordered_map<unsigned int, DirectX::XMFLOAT4X4> _newWorldTM;		// 로컬 행렬을 통해 재 조립한 월드 행렬

		std::unordered_map<unsigned int, DirectX::XMFLOAT4X4> _slerpTM;			// 선형 보간을 위해 LocalTM을 복사해두고 여기서 처리해 LocalTM을 건드리지 않기 위한 행렬
	};

	struct Mesh : ASEOBJECT
	{
		Mesh();
		~Mesh();

		unsigned int m_TotalVertexNum;
		unsigned int m_TotalFaceNum;

		std::vector<Vertex*> m_VertexList;
		std::vector<Face*> m_FaceList;
		std::vector<Tvert*> m_TvertList;
		std::vector<Tface*> m_TfaceList;

		unsigned int m_TotalBoneNum;				// 한 오브젝트에 한개만 존재하는건지 ? 존재한다면 Mesh가 아니라 Parser에서 가지고 있도록 변경 예정
		std::vector<std::string> m_BoneNameList;	// 한 오브젝트에 한개만 존재하는건지 ? 존재한다면 Mesh가 아니라 Parser에서 가지고 있도록 변경 예정
		std::vector<Mesh*> m_BoneList;				// 한 오브젝트에 한개만 존재하는건지 ? 존재한다면 Mesh가 아니라 Parser에서 가지고 있도록 변경 예정
		bool m_isBone;
		unsigned int m_TempBoneIndex;
	};

	struct Helper : ASEOBJECT
	{
		Helper();
	};

	struct Shape : ASEOBJECT
	{
		Shape();
	};

	struct Material
	{
		Material();

		std::string m_name;

		DirectX::XMFLOAT4 m_ambient;
		DirectX::XMFLOAT4 m_diffuse;
		DirectX::XMFLOAT4 m_specular;
	};
}

