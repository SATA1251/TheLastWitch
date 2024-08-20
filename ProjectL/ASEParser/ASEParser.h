/// <summary>
/// 
/// ASE �Ľ��� ������ Ŭ����
/// �����Բ��� ���� parser Ŭ������ �����ϸ� ����Ͽ� �ʿ��� �κи� �����ϰ� ������ ����
/// �⺻ �޽� -> �ؽ��� -> �ִϸ��̼� ������ �ʿ��� ���� ä������ ����
/// 
/// 2023.06.22 YJH

#pragma once
#include "ASEFile.h"
#include "ParsingDataStruct.h"

class ASEParser
{
public:
	ASEParser();
	~ASEParser();

public:
	enum class eObjectState
	{
		NONE,
		GEOMOBJECT,
		HELPEROBEJCT,
		SHAPEOBJECT
	};

public:
	void Load(const char* fileName);
	std::vector<ASE_Parser::Mesh*> GetMeshList();
	std::vector<ASE_Parser::Helper*> GetHelperList();
	std::vector<ASE_Parser::Shape*> GetShapeList();
	std::vector<ASE_Parser::Material*> GetMaterialList();
	std::string GetFileName();
	bool GetIsAnimated();
	int GetStartFrame();
	int GetFinalFrame();
	bool GetBoneExist();

	void AddMeshCount();
	void DeleteMeshInfo(int meshIndex);
	int GetMeshCount();

public:
	void SetSpecificNewWorldTM(ASE_Parser::ASEOBJECT* pMesh, int meshIndex);	// pMesh�� ���� �θ� mesh���� WolrdTM�� ���Ӱ� ���
	void SetSpecificSlerpTM(ASE_Parser::ASEOBJECT* pMesh, int meshIndex);

	void SetLocalTM();				// WorldTM�� �̿��Ͽ� LocalTM�� ����
	void SetNewWorldTM();			// LocalTM�� �̿��Ͽ� ���ο� WorldTM�� ����

private:
	void Initialize();
	void Reset();
	void Parsing_ALL(int depth);
	void VertexSplit();
	void ApplyTextureNormal();
	void SetRelationShip(const eObjectState eState, ASE_Parser::ASEOBJECT* baseObject, ASE_Parser::ASEOBJECT* pASEOBJECT, const std::string& parentName);
	void SetRelationShip(ASE_Parser::ASEOBJECT* baseObject, ASE_Parser::ASEOBJECT* pASEOBJECT);

private:
	void CreateOneMeshToList();
	void CreateOneVertexToList();
	void CreateOneVertexToList(int index);
	void CreateOneFaceToList();
	void CreateOneTvertToList();
	void CreateOneTfaceToList();
	void CreateOneHelperToList();
	void CreateOneShapeToList();

private:
	void SetOriginalWorldTM();		// ���� TM_ROW ���� �̿��� ���� WorldTM�� ����
	void SetToLocalPosition();		// ���� ��ǥ���� OriginWorld�� ���� ���Ͽ� ���÷� ��ȯ
	void SetToNewWorldPosition();	// ���ÿ� �ִ� ������Ʈ�� ���� ����� ���� ����� �̿��� ��ȯ

	void SetBoneList();				// �� �̸��� ������ ����Ʈ���� �� �̸��� ���� �޽��� �����ͷ� ������ ����Ʈ�� ��ȯ

private:
	int Parsing_NumberInt();
	float Parsing_NumberFloat();
	DirectX::SimpleMath::Vector3 Parsing_NumberVector3();
	DirectX::SimpleMath::Vector4 Parsing_NumberVector4();
	LPSTR Parsing_String();

private:
	ASE::CASELexer* m_Lexer;

	std::string m_fileName;

	std::vector<ASE_Parser::Material*> m_MaterialList;

	std::vector<ASE_Parser::Mesh*> m_MeshList;
	std::vector<ASE_Parser::Helper*> m_HelperList;
	std::vector<ASE_Parser::Shape*> m_ShapeList;

	char m_TokenString[256];

	eObjectState m_ObjectState;

	bool m_isAnimated;

	int m_StartFrame;

	bool m_isStartFrameExist;

	int m_FinalFrame;

	bool m_BoneExist;

	int m_MaterialIndex;

	int _meshCount;
};