/// <summary>
/// 
/// ASE 파싱을 진행할 클래스
/// 교수님께서 만든 parser 클래스를 공부하며 모방하여 필요한 부분만 간단하게 구현할 예정
/// 기본 메쉬 -> 텍스쳐 -> 애니메이션 순서로 필요한 것을 채워넣을 예정
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
	void SetSpecificNewWorldTM(ASE_Parser::ASEOBJECT* pMesh, int meshIndex);	// pMesh와 그의 부모 mesh들의 WolrdTM만 새롭게 계산
	void SetSpecificSlerpTM(ASE_Parser::ASEOBJECT* pMesh, int meshIndex);

	void SetLocalTM();				// WorldTM을 이용하여 LocalTM을 구함
	void SetNewWorldTM();			// LocalTM을 이용하여 새로운 WorldTM을 구함

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
	void SetOriginalWorldTM();		// 기존 TM_ROW 값을 이용해 원래 WorldTM을 구함
	void SetToLocalPosition();		// 기존 좌표에서 OriginWorld의 역을 곱하여 로컬로 변환
	void SetToNewWorldPosition();	// 로컬에 있는 오브젝트를 직접 계산한 월드 행렬을 이용해 변환

	void SetBoneList();				// 본 이름을 저장한 리스트에서 그 이름을 가진 메쉬를 포인터로 저장한 리스트로 변환

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