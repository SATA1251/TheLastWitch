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

		bool isVertexSet;	// ���ؽ� �������� ä�����°�
		std::vector<unsigned int> m_splitList;

		std::vector<BoneWeight> m_BoneWeightList;

		// DirectX::SimpleMath::Vector2 m_Color;
		// color ���� �� �Ⱦ��ϰ� ������ ���� ���������� �޸� �����̹Ƿ� �ϴ� �ּ�ó��
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
		// face�� ���� ���⶧���� ���� ������ ���� �ʿ䰡 �����Ű��� �ּ�ó��

		DirectX::SimpleMath::Vector3 m_UVW;
	};

	struct Tface
	{
		Tface();

		// unsigned int m_TfaceNum;
		// face�� ���� ���⶧���� ���� ������ ���� �ʿ䰡 �����Ű��� �ּ�ó��

		std::vector<unsigned int> m_indexList;
	};

	// �޽�, ����, ������ ������Ʈ�� ���� ���� -> ��� ���踦 ���� �θ�� �ڽ��� �����͸� �θ� ��� ���ͷ� ���� �ֱ� ����
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

		DirectX::XMFLOAT4X4 _originWorldTM;	// TM_ROW�� ������ ���� ���
		DirectX::XMFLOAT4X4 _originLocalTM;	// �ִϸ��̼� �ʱ�ȭ�� ���� ������ ���� �⺻ ���� ���

		std::unordered_map<unsigned int, DirectX::XMFLOAT4X4> _localTM;			// ���� ����� ���� ����� ���� ���
		std::unordered_map<unsigned int, DirectX::XMFLOAT4X4> _newWorldTM;		// ���� ����� ���� �� ������ ���� ���

		std::unordered_map<unsigned int, DirectX::XMFLOAT4X4> _slerpTM;			// ���� ������ ���� LocalTM�� �����صΰ� ���⼭ ó���� LocalTM�� �ǵ帮�� �ʱ� ���� ���
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

		unsigned int m_TotalBoneNum;				// �� ������Ʈ�� �Ѱ��� �����ϴ°��� ? �����Ѵٸ� Mesh�� �ƴ϶� Parser���� ������ �ֵ��� ���� ����
		std::vector<std::string> m_BoneNameList;	// �� ������Ʈ�� �Ѱ��� �����ϴ°��� ? �����Ѵٸ� Mesh�� �ƴ϶� Parser���� ������ �ֵ��� ���� ����
		std::vector<Mesh*> m_BoneList;				// �� ������Ʈ�� �Ѱ��� �����ϴ°��� ? �����Ѵٸ� Mesh�� �ƴ϶� Parser���� ������ �ֵ��� ���� ����
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

