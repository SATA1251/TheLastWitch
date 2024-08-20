#include "ASEParser.h"
#include "../ArkEngine/ResourceManager.h"

ASEParser::ASEParser()
	: m_Lexer(nullptr), m_fileName(), m_TokenString{ 0, }, m_ObjectState(eObjectState::NONE), m_isAnimated(false), m_StartFrame(0), m_isStartFrameExist(false), m_FinalFrame(0), m_BoneExist(false), m_MaterialIndex(0),
	_meshCount(0)
{
	Initialize();
}

ASEParser::~ASEParser()
{

}

void ASEParser::Initialize()
{
	m_Lexer = new ASE::CASELexer;
}

void ASEParser::Load(const char* fileName)
{
	LPSTR tempChar = (LPSTR)fileName;
	if (!m_Lexer->Open(tempChar))
	{
		TRACE("������ ���� �� ���� �߻�");
	}

	Parsing_ALL(0);
	VertexSplit();

	SetBoneList();

	ApplyTextureNormal();

	SetOriginalWorldTM();

	SetLocalTM();

	for (auto index : m_ShapeList)
	{
		index->_localTM.insert(std::make_pair(_meshCount, index->_originLocalTM));
		index->_newWorldTM.insert(std::make_pair(_meshCount, index->_localTM.at(_meshCount)));
		index->_slerpTM.insert(std::make_pair(_meshCount, index->_localTM.at(_meshCount)));
	}

	for (auto index : m_HelperList)
	{
		index->_localTM.insert(std::make_pair(_meshCount, index->_originLocalTM));
		index->_newWorldTM.insert(std::make_pair(_meshCount, index->_localTM.at(_meshCount)));
		index->_slerpTM.insert(std::make_pair(_meshCount, index->_localTM.at(_meshCount)));
	}

	for (auto index : m_MeshList)
	{
		index->_localTM.insert(std::make_pair(_meshCount, index->_originLocalTM));
		index->_newWorldTM.insert(std::make_pair(_meshCount, index->_localTM.at(_meshCount)));
		index->_slerpTM.insert(std::make_pair(_meshCount, index->_localTM.at(_meshCount)));
	}

	SetToLocalPosition();

	SetNewWorldTM();

	//ArkEngine::ResourceManager::GetInstance()->SetResource(fileName, this);
	//Reset();
}

std::vector<ASE_Parser::Mesh*> ASEParser::GetMeshList()
{
	return m_MeshList;
}

std::vector<ASE_Parser::Helper*> ASEParser::GetHelperList()
{
	return m_HelperList;
}

std::vector<ASE_Parser::Shape*> ASEParser::GetShapeList()
{
	return m_ShapeList;
}

std::vector<ASE_Parser::Material*> ASEParser::GetMaterialList()
{
	return m_MaterialList;
}

std::string ASEParser::GetFileName()
{
	return m_fileName;
}

void ASEParser::Reset()
{
	m_fileName = "";
	std::vector<ASE_Parser::Material*> newMaterial;
	m_MaterialList.swap(newMaterial);
	std::vector<ASE_Parser::Mesh*> newMesh;
	m_MeshList.swap(newMesh);
	std::vector<ASE_Parser::Helper*> newHelper;
	m_HelperList.swap(newHelper);
	std::vector<ASE_Parser::Shape*> newShape;
	m_ShapeList.swap(newShape);
	m_ObjectState = eObjectState::NONE;
	m_isAnimated = false;
	m_StartFrame = 0;
	m_isStartFrameExist = false;
	m_FinalFrame = 0;
	m_BoneExist = false;
	m_MaterialIndex = 0;
}

void ASEParser::Parsing_ALL(int depth)
{
	LONG nowtoken;

	int tempInt = 0;
	std::string tempString;
	tempString.clear();
	int faceIndex = 0;
	DirectX::SimpleMath::Vector3 tempVec3;
	DirectX::SimpleMath::Vector4 tempVec4;
	bool tempBool = false;

	while (nowtoken = m_Lexer->GetToken(m_TokenString), nowtoken != TOKEND_BLOCK_END)
	{
		switch (nowtoken)
		{
			case TOKEND_BLOCK_START:
				Parsing_ALL(depth++);
				break;

			case TOKENR_SCENE_FILENAME:
				m_fileName = Parsing_String();
				break;

			case TOKENR_MATERIAL_COUNT:
				tempInt = Parsing_NumberInt();

				for (int i = 0; i < tempInt; i++)
				{
					m_MaterialList.emplace_back(new ASE_Parser::Material);
				}
				break;

			case TOKENR_MATERIAL:
				m_MaterialIndex = Parsing_NumberInt();
				break;

			case TOKENR_MATERIAL_NAME:
				m_MaterialList[m_MaterialIndex]->m_name = Parsing_String();
				break;

			case TOKENR_MATERIAL_AMBIENT:
				m_MaterialList[m_MaterialIndex]->m_ambient.x = Parsing_NumberFloat();
				m_MaterialList[m_MaterialIndex]->m_ambient.y = Parsing_NumberFloat();
				m_MaterialList[m_MaterialIndex]->m_ambient.z = Parsing_NumberFloat();
				break;

			case TOKENR_MATERIAL_DIFFUSE:
				m_MaterialList[m_MaterialIndex]->m_diffuse.x = Parsing_NumberFloat();
				m_MaterialList[m_MaterialIndex]->m_diffuse.y = Parsing_NumberFloat();
				m_MaterialList[m_MaterialIndex]->m_diffuse.z = Parsing_NumberFloat();
				break;

			case TOKENR_MATERIAL_SPECULAR:
				m_MaterialList[m_MaterialIndex]->m_specular.x = Parsing_NumberFloat();
				m_MaterialList[m_MaterialIndex]->m_specular.y = Parsing_NumberFloat();
				m_MaterialList[m_MaterialIndex]->m_specular.z = Parsing_NumberFloat();
				break;

			case TOKENR_GEOMOBJECT:
				m_ObjectState = eObjectState::GEOMOBJECT;
				CreateOneMeshToList();
				break;

			case TOKENR_HELPEROBJECT:
				m_ObjectState = eObjectState::HELPEROBEJCT;
				CreateOneHelperToList();
				break;

			case TOKENR_SHAPEOBJECT:
				m_ObjectState = eObjectState::SHAPEOBJECT;
				CreateOneShapeToList();
				break;

				/// GEOMOBJECT, HELPER, SHPAEE ������ ��ҵ�
			case TOKENR_NODE_NAME:
				tempString = Parsing_String();
				switch (m_ObjectState)
				{
					case eObjectState::GEOMOBJECT:
						m_MeshList[m_MeshList.size() - 1]->m_NodeName = tempString;
						break;

					case eObjectState::HELPEROBEJCT:
						m_HelperList[m_HelperList.size() - 1]->m_NodeName = tempString;
						break;

					case eObjectState::SHAPEOBJECT:
						m_ShapeList[m_ShapeList.size() - 1]->m_NodeName = tempString;
						break;
				}
				tempString.clear();
				break;

			case TOKENR_NODE_PARENT:
				tempString = Parsing_String();
				switch (m_ObjectState)
				{
					case eObjectState::GEOMOBJECT:
						m_MeshList[m_MeshList.size() - 1]->m_NodeParent = tempString;
						SetRelationShip(m_ObjectState, (ASE_Parser::ASEOBJECT*)m_MeshList[m_MeshList.size() - 1], (ASE_Parser::ASEOBJECT*)m_MeshList[m_MeshList.size() - 1], tempString);
						break;

					case eObjectState::HELPEROBEJCT:
						m_HelperList[m_HelperList.size() - 1]->m_NodeParent = tempString;
						SetRelationShip(m_ObjectState, (ASE_Parser::ASEOBJECT*)m_HelperList[m_HelperList.size() - 1], (ASE_Parser::ASEOBJECT*)m_HelperList[m_HelperList.size() - 1], tempString);
						break;

					case eObjectState::SHAPEOBJECT:
						m_ShapeList[m_ShapeList.size() - 1]->m_NodeParent = tempString;
						SetRelationShip(m_ObjectState, (ASE_Parser::ASEOBJECT*)m_ShapeList[m_ShapeList.size() - 1], (ASE_Parser::ASEOBJECT*)m_ShapeList[m_ShapeList.size() - 1], tempString);
						break;
				}
				tempString.clear();
				break;

				// ROW 0~4�� ���� ����� ���鶧 ��ġ���ְ� ���⼭�� ���� �״�� �޾ƿ����� ��
			case TOKENR_TM_ROW0:
				switch (m_ObjectState)
				{
					case eObjectState::GEOMOBJECT:
						m_MeshList[m_MeshList.size() - 1]->m_Tm_Row0 = Parsing_NumberVector3();
						break;

					case eObjectState::HELPEROBEJCT:
						m_HelperList[m_HelperList.size() - 1]->m_Tm_Row0 = Parsing_NumberVector3();
						break;

					case eObjectState::SHAPEOBJECT:
						m_ShapeList[m_ShapeList.size() - 1]->m_Tm_Row0 = Parsing_NumberVector3();
						break;
				}
				break;

			case TOKENR_TM_ROW1:
				switch (m_ObjectState)
				{
					case eObjectState::GEOMOBJECT:
						m_MeshList[m_MeshList.size() - 1]->m_Tm_Row1 = Parsing_NumberVector3();
						break;

					case eObjectState::HELPEROBEJCT:
						m_HelperList[m_HelperList.size() - 1]->m_Tm_Row1 = Parsing_NumberVector3();
						break;

					case eObjectState::SHAPEOBJECT:
						m_ShapeList[m_ShapeList.size() - 1]->m_Tm_Row1 = Parsing_NumberVector3();
						break;
				}
				break;

			case TOKENR_TM_ROW2:
				switch (m_ObjectState)
				{
					case eObjectState::GEOMOBJECT:
						m_MeshList[m_MeshList.size() - 1]->m_Tm_Row2 = Parsing_NumberVector3();
						break;

					case eObjectState::HELPEROBEJCT:
						m_HelperList[m_HelperList.size() - 1]->m_Tm_Row2 = Parsing_NumberVector3();
						break;

					case eObjectState::SHAPEOBJECT:
						m_ShapeList[m_ShapeList.size() - 1]->m_Tm_Row2 = Parsing_NumberVector3();
						break;
				}
				break;

			case TOKENR_TM_ROW3:
				switch (m_ObjectState)
				{
					case eObjectState::GEOMOBJECT:
						m_MeshList[m_MeshList.size() - 1]->m_Tm_Row3 = Parsing_NumberVector3();
						break;

					case eObjectState::HELPEROBEJCT:
						m_HelperList[m_HelperList.size() - 1]->m_Tm_Row3 = Parsing_NumberVector3();
						break;

					case eObjectState::SHAPEOBJECT:
						m_ShapeList[m_ShapeList.size() - 1]->m_Tm_Row3 = Parsing_NumberVector3();
						break;
				}
				break;

			case TOKENR_TM_POS:
				switch (m_ObjectState)
				{
					case eObjectState::GEOMOBJECT:
						m_MeshList[m_MeshList.size() - 1]->m_Tm_Pos = Parsing_NumberVector3();
						break;

					case eObjectState::HELPEROBEJCT:
						m_HelperList[m_HelperList.size() - 1]->m_Tm_Pos = Parsing_NumberVector3();
						break;

					case eObjectState::SHAPEOBJECT:
						m_ShapeList[m_ShapeList.size() - 1]->m_Tm_Pos = Parsing_NumberVector3();
						break;
				}
				break;

			case TOKENR_TM_ROTAXIS:
				switch (m_ObjectState)
				{
					case eObjectState::GEOMOBJECT:
						m_MeshList[m_MeshList.size() - 1]->m_Tm_RotAxis = Parsing_NumberVector3();
						break;

					case eObjectState::HELPEROBEJCT:
						m_HelperList[m_HelperList.size() - 1]->m_Tm_RotAxis = Parsing_NumberVector3();
						break;

					case eObjectState::SHAPEOBJECT:
						m_ShapeList[m_ShapeList.size() - 1]->m_Tm_RotAxis = Parsing_NumberVector3();
						break;
				}
				break;

			case TOKENR_TM_ROTANGLE:
				switch (m_ObjectState)
				{
					case eObjectState::GEOMOBJECT:
						m_MeshList[m_MeshList.size() - 1]->m_Tm_RotAngle = Parsing_NumberFloat();
						break;

					case eObjectState::HELPEROBEJCT:
						m_HelperList[m_HelperList.size() - 1]->m_Tm_RotAngle = Parsing_NumberFloat();
						break;

					case eObjectState::SHAPEOBJECT:
						m_ShapeList[m_ShapeList.size() - 1]->m_Tm_RotAngle = Parsing_NumberFloat();
						break;
				}
				break;

			case TOKENR_TM_SCALE:
				switch (m_ObjectState)
				{
					case eObjectState::GEOMOBJECT:
						m_MeshList[m_MeshList.size() - 1]->m_Tm_Scale = Parsing_NumberVector3();
						break;

					case eObjectState::HELPEROBEJCT:
						m_HelperList[m_HelperList.size() - 1]->m_Tm_Scale = Parsing_NumberVector3();
						break;

					case eObjectState::SHAPEOBJECT:
						m_ShapeList[m_ShapeList.size() - 1]->m_Tm_Scale = Parsing_NumberVector3();
						break;
				}
				break;

			case TOKENR_TM_SCALEAXIS:
				switch (m_ObjectState)
				{
					case eObjectState::GEOMOBJECT:
						m_MeshList[m_MeshList.size() - 1]->m_Tm_ScaleAxis = Parsing_NumberVector3();
						break;

					case eObjectState::HELPEROBEJCT:
						m_HelperList[m_HelperList.size() - 1]->m_Tm_ScaleAxis = Parsing_NumberVector3();
						break;

					case eObjectState::SHAPEOBJECT:
						m_ShapeList[m_ShapeList.size() - 1]->m_Tm_ScaleAxis = Parsing_NumberVector3();
						break;
				}
				break;

			case TOKENR_TM_SCALEAXISANG:
				switch (m_ObjectState)
				{
					case eObjectState::GEOMOBJECT:
						m_MeshList[m_MeshList.size() - 1]->m_Tm_ScaleAxisAng = Parsing_NumberFloat();
						break;

					case eObjectState::HELPEROBEJCT:
						m_HelperList[m_HelperList.size() - 1]->m_Tm_ScaleAxisAng = Parsing_NumberFloat();
						break;

					case eObjectState::SHAPEOBJECT:
						m_ShapeList[m_ShapeList.size() - 1]->m_Tm_ScaleAxisAng = Parsing_NumberFloat();
						break;
				}
				break;

			case TOKENR_TM_ANIMATION:
				m_isAnimated = true;
				break;

			case TOKENR_CONTROL_POS_SAMPLE:
				tempInt = Parsing_NumberInt();
				tempVec3.x = Parsing_NumberFloat();
				tempVec3.z = Parsing_NumberFloat();
				tempVec3.y = Parsing_NumberFloat();

				// �ִϸ��̼��� ���� �޽� ��ü������ ���� ������ ����
				if (m_isStartFrameExist == false)
				{
					m_StartFrame = tempInt;
					m_isStartFrameExist = true;
				}

				if (m_isStartFrameExist == true)
				{
					if (m_StartFrame > tempInt)
					{
						m_StartFrame = tempInt;
					}
				}

				// �ִϸ��̼��� ���� �޽� ��ü������ ������ ������ ���� 
				if (m_FinalFrame < tempInt)
				{
					m_FinalFrame = tempInt;
				}

				switch (m_ObjectState)
				{

					case eObjectState::GEOMOBJECT:
						m_MeshList[m_MeshList.size() - 1]->m_Animation_Pos.insert(std::make_pair(tempInt, tempVec3));
						m_MeshList[m_MeshList.size() - 1]->m_Animation_Pos_Index.push_back(tempInt);
						break;

					case eObjectState::HELPEROBEJCT:
						m_HelperList[m_HelperList.size() - 1]->m_Animation_Pos.insert(std::make_pair(tempInt, tempVec3));
						m_HelperList[m_HelperList.size() - 1]->m_Animation_Pos_Index.push_back(tempInt);
						break;

					case eObjectState::SHAPEOBJECT:
						m_ShapeList[m_ShapeList.size() - 1]->m_Animation_Pos.insert(std::make_pair(tempInt, tempVec3));
						m_ShapeList[m_ShapeList.size() - 1]->m_Animation_Pos_Index.push_back(tempInt);
						break;
				}
				break;

			case TOKENR_CONTROL_ROT_SAMPLE:
				tempInt = Parsing_NumberInt();
				tempVec4.x = Parsing_NumberFloat();
				tempVec4.z = Parsing_NumberFloat();
				tempVec4.y = Parsing_NumberFloat();
				tempVec4.w = Parsing_NumberFloat();
				if (m_FinalFrame < tempInt)
				{
					m_FinalFrame = tempInt;
				}
				switch (m_ObjectState)
				{
					case eObjectState::GEOMOBJECT:
						m_MeshList[m_MeshList.size() - 1]->m_Animation_Rot.insert(std::make_pair(tempInt, tempVec4));
						m_MeshList[m_MeshList.size() - 1]->m_Animation_Rot_Index.push_back(tempInt);
						break;

					case eObjectState::HELPEROBEJCT:
						m_HelperList[m_HelperList.size() - 1]->m_Animation_Rot.insert(std::make_pair(tempInt, tempVec4));
						m_HelperList[m_HelperList.size() - 1]->m_Animation_Rot_Index.push_back(tempInt);

						break;

					case eObjectState::SHAPEOBJECT:
						m_ShapeList[m_ShapeList.size() - 1]->m_Animation_Rot.insert(std::make_pair(tempInt, tempVec4));
						m_ShapeList[m_ShapeList.size() - 1]->m_Animation_Rot_Index.push_back(tempInt);
						break;
				}
				break;

				/// GEOMOBJECT���� ��� - Mesh
			case TOKENR_MESH_NUMVERTEX:
				//m_MeshList[m_MeshList.size() - 1]->m_TotalVertexNum = Parsing_NumberInt();
				break;

			case TOKENR_MESH_NUMFACES:
				m_MeshList[m_MeshList.size() - 1]->m_TotalFaceNum = Parsing_NumberInt();
				break;

			case TOKENR_MESH_VERTEX:
				CreateOneVertexToList();

				tempInt = Parsing_NumberInt();
				// x,z,y ������ �����⿡ �־��ִ� ������ �ٲ�� ��
				m_MeshList[m_MeshList.size() - 1]->m_VertexList[tempInt]->m_Position.x = Parsing_NumberFloat();
				m_MeshList[m_MeshList.size() - 1]->m_VertexList[tempInt]->m_Position.z = Parsing_NumberFloat();
				m_MeshList[m_MeshList.size() - 1]->m_VertexList[tempInt]->m_Position.y = Parsing_NumberFloat();

				tempInt = 0;
				break;

			case TOKENR_MESH_FACE:
				CreateOneFaceToList();

				// 0: �� :�� �������� ���� ���ںκи� ������
				tempInt = Parsing_NumberInt();

				// x,z,y ������ �����⿡ �־��ִ� ������ �ٲ�� �� -> 0,2,1

				for (int i = 0; i < 3; i++)
				{
					m_MeshList[m_MeshList.size() - 1]->m_FaceList[tempInt]->m_indexList.push_back(0);
				}

				Parsing_String();	// �ʿ���� ���� A: ������
				m_MeshList[m_MeshList.size() - 1]->m_FaceList[tempInt]->m_indexList[0] = Parsing_NumberInt();
				Parsing_String();	// �ʿ���� ���� b: ������
				m_MeshList[m_MeshList.size() - 1]->m_FaceList[tempInt]->m_indexList[2] = Parsing_NumberInt();
				Parsing_String();   // �ʿ���� ���� C: ������
				m_MeshList[m_MeshList.size() - 1]->m_FaceList[tempInt]->m_indexList[1] = Parsing_NumberInt();

				tempInt = 0;
				break;

			case TOKENR_MESH_NUMTVERTEX:
				// ��¥�� ���̽� ���ڶ� �Ȱ����� ���� �ٸ� ������ ���� �ʿ䰡 �����Ű��Ƽ� �ϴ� ó������
				break;

			case TOKENR_MESH_TVERT:
				CreateOneTvertToList();

				tempInt = Parsing_NumberInt();
				m_MeshList[m_MeshList.size() - 1]->m_TvertList[tempInt]->m_UVW = Parsing_NumberVector3();
				m_MeshList[m_MeshList.size() - 1]->m_TvertList[tempInt]->m_UVW.y *= -1;
				tempInt = 0;
				break;

			case TOKENR_MESH_NUMTVFACES:
				// ��¥�� ���̽� ���ڶ� �Ȱ����� ���� �ٸ� ������ ���� �ʿ䰡 �����Ű��Ƽ� �ϴ� ó������
				break;

			case TOKENR_MESH_TFACE:
				CreateOneTfaceToList();

				// x,z,y ������ �����⿡ �־��ִ� ������ �ٲ�� �� -> 0,2,1
				tempInt = Parsing_NumberInt();

				for (int i = 0; i < 3; i++)
				{
					m_MeshList[m_MeshList.size() - 1]->m_TfaceList[tempInt]->m_indexList.push_back(0);
				}

				m_MeshList[m_MeshList.size() - 1]->m_TfaceList[tempInt]->m_indexList[0] = Parsing_NumberInt();
				m_MeshList[m_MeshList.size() - 1]->m_TfaceList[tempInt]->m_indexList[2] = Parsing_NumberInt();
				m_MeshList[m_MeshList.size() - 1]->m_TfaceList[tempInt]->m_indexList[1] = Parsing_NumberInt();

				tempInt = 0;
				break;

			case TOKENR_MESH_FACENORMAL:
				tempInt = Parsing_NumberInt();
				faceIndex = tempInt;
				// x,z,y ������ �����⿡ �־��ִ� ������ �ٲ�� ��
				m_MeshList[m_MeshList.size() - 1]->m_FaceList[tempInt]->m_Normal.x = Parsing_NumberFloat();
				m_MeshList[m_MeshList.size() - 1]->m_FaceList[tempInt]->m_Normal.z = Parsing_NumberFloat();
				m_MeshList[m_MeshList.size() - 1]->m_FaceList[tempInt]->m_Normal.y = Parsing_NumberFloat();

				tempInt = 0;
				break;

			case TOKENR_MESH_VERTEXNORMAL:
				tempInt = Parsing_NumberInt();
				tempVec3 = Parsing_NumberVector3();

				if (m_MeshList[m_MeshList.size() - 1]->m_VertexList[tempInt]->isVertexSet == false)
				{
					// x,z,y ������ �����⿡ �־��ִ� ������ �ٲ�� ��
					m_MeshList[m_MeshList.size() - 1]->m_VertexList[tempInt]->m_Normal.x = tempVec3.x;
					m_MeshList[m_MeshList.size() - 1]->m_VertexList[tempInt]->m_Normal.z = tempVec3.y;
					m_MeshList[m_MeshList.size() - 1]->m_VertexList[tempInt]->m_Normal.y = tempVec3.z;
					m_MeshList[m_MeshList.size() - 1]->m_VertexList[tempInt]->isVertexSet = true;

					// ���� �������� ���ؽ��� ���� ���ø��� ���ؽ��� �ε��� ����Ʈ�� ������ �ֱ� ����
					m_MeshList[m_MeshList.size() - 1]->m_VertexList[tempInt]->m_splitList.push_back(tempInt);
				}

				// �븻���� �������µ� �̹� �ش� ���ؽ��� �븻���� �����ϸ�, �� �븻 ���� ������ ���̽� �븻���� �ٸ� ���
				else if (m_MeshList[m_MeshList.size() - 1]->m_VertexList[tempInt]->isVertexSet == true)
				{
					// �о���� �븻 ���� �ش� ���ؽ��� �븻 ���� ��ġ���� �ʰ�
					if (m_MeshList[m_MeshList.size() - 1]->m_VertexList[tempInt]->m_Normal.x != tempVec3.x ||
						m_MeshList[m_MeshList.size() - 1]->m_VertexList[tempInt]->m_Normal.y != tempVec3.z ||
						m_MeshList[m_MeshList.size() - 1]->m_VertexList[tempInt]->m_Normal.z != tempVec3.y)
					{
						// �������� ���ؽ��� �������� �ʴ´ٸ� (���ø� ����Ʈ ���� �ڱ��ڽ� ���̶��)
						if (m_MeshList[m_MeshList.size() - 1]->m_VertexList[tempInt]->m_splitList.size() <= 1)
						{
							// ���ο� �븻�� ���� ���ؽ��� �߰��ؾ� ��
							CreateOneVertexToList();

							// ���� ���� ���ؽ��� ��� ������ �Ű� ����
							m_MeshList[m_MeshList.size() - 1]->m_VertexList[m_MeshList[m_MeshList.size() - 1]->m_VertexList.size() - 1]->m_Position.x = m_MeshList[m_MeshList.size() - 1]->m_VertexList[tempInt]->m_Position.x;
							m_MeshList[m_MeshList.size() - 1]->m_VertexList[m_MeshList[m_MeshList.size() - 1]->m_VertexList.size() - 1]->m_Position.z = m_MeshList[m_MeshList.size() - 1]->m_VertexList[tempInt]->m_Position.z;
							m_MeshList[m_MeshList.size() - 1]->m_VertexList[m_MeshList[m_MeshList.size() - 1]->m_VertexList.size() - 1]->m_Position.y = m_MeshList[m_MeshList.size() - 1]->m_VertexList[tempInt]->m_Position.y;

							m_MeshList[m_MeshList.size() - 1]->m_VertexList[m_MeshList[m_MeshList.size() - 1]->m_VertexList.size() - 1]->m_Normal.x = tempVec3.x;
							m_MeshList[m_MeshList.size() - 1]->m_VertexList[m_MeshList[m_MeshList.size() - 1]->m_VertexList.size() - 1]->m_Normal.z = tempVec3.y;
							m_MeshList[m_MeshList.size() - 1]->m_VertexList[m_MeshList[m_MeshList.size() - 1]->m_VertexList.size() - 1]->m_Normal.y = tempVec3.z;

							m_MeshList[m_MeshList.size() - 1]->m_VertexList[m_MeshList[m_MeshList.size() - 1]->m_VertexList.size() - 1]->m_Texture = m_MeshList[m_MeshList.size() - 1]->m_VertexList[tempInt]->m_Texture;

							m_MeshList[m_MeshList.size() - 1]->m_VertexList[m_MeshList[m_MeshList.size() - 1]->m_VertexList.size() - 1]->isVertexSet = true;

							// ���� �������� ���ؽ��� ���� ���ø��� ���ؽ��� �ε��� ����Ʈ�� ������ �ֱ� ����
							m_MeshList[m_MeshList.size() - 1]->m_VertexList[tempInt]->m_splitList.push_back((unsigned int)m_MeshList[m_MeshList.size() - 1]->m_VertexList.size() - 1);
						}

						// ���ø��� ���ؽ��� ������ ���
						else if (m_MeshList[m_MeshList.size() - 1]->m_VertexList[tempInt]->m_splitList.size() > 1)
						{
							// ���ø� ����Ʈ�� ���鼭 ��ȸ
							for (int i = 0; i < m_MeshList[m_MeshList.size() - 1]->m_VertexList[tempInt]->m_splitList.size(); i++)
							{
								// ���ø� ����Ʈ���� ���� �о���� �븻 ���� ���� ���ؽ��� �ִ� ���
								if (m_MeshList[m_MeshList.size() - 1]->m_VertexList[m_MeshList[m_MeshList.size() - 1]->m_VertexList[tempInt]->m_splitList[i]]->m_Normal.x == tempVec3.x &&
									m_MeshList[m_MeshList.size() - 1]->m_VertexList[m_MeshList[m_MeshList.size() - 1]->m_VertexList[tempInt]->m_splitList[i]]->m_Normal.y == tempVec3.z &&
									m_MeshList[m_MeshList.size() - 1]->m_VertexList[m_MeshList[m_MeshList.size() - 1]->m_VertexList[tempInt]->m_splitList[i]]->m_Normal.z == tempVec3.y)
								{
									tempBool = true;
									break;
								}
							}
							// ���ø� ����Ʈ���� ���� �о���� �븻 ���� ���� ���ؽ��� ���� ��� (������ �ߺ��� ���ؽ��� ���� ���)���� ���ؽ� ���ø�
							if (tempBool == false)
							{
								// ���ο� �븻�� ���� ���ؽ��� �߰��ؾ� ��
								CreateOneVertexToList();

								// ���� ���� ���ؽ��� ��� ������ �Ű� ����
								m_MeshList[m_MeshList.size() - 1]->m_VertexList[m_MeshList[m_MeshList.size() - 1]->m_VertexList.size() - 1]->m_Position.x = m_MeshList[m_MeshList.size() - 1]->m_VertexList[tempInt]->m_Position.x;
								m_MeshList[m_MeshList.size() - 1]->m_VertexList[m_MeshList[m_MeshList.size() - 1]->m_VertexList.size() - 1]->m_Position.z = m_MeshList[m_MeshList.size() - 1]->m_VertexList[tempInt]->m_Position.z;
								m_MeshList[m_MeshList.size() - 1]->m_VertexList[m_MeshList[m_MeshList.size() - 1]->m_VertexList.size() - 1]->m_Position.y = m_MeshList[m_MeshList.size() - 1]->m_VertexList[tempInt]->m_Position.y;

								m_MeshList[m_MeshList.size() - 1]->m_VertexList[m_MeshList[m_MeshList.size() - 1]->m_VertexList.size() - 1]->m_Normal.x = tempVec3.x;
								m_MeshList[m_MeshList.size() - 1]->m_VertexList[m_MeshList[m_MeshList.size() - 1]->m_VertexList.size() - 1]->m_Normal.z = tempVec3.y;
								m_MeshList[m_MeshList.size() - 1]->m_VertexList[m_MeshList[m_MeshList.size() - 1]->m_VertexList.size() - 1]->m_Normal.y = tempVec3.z;

								m_MeshList[m_MeshList.size() - 1]->m_VertexList[m_MeshList[m_MeshList.size() - 1]->m_VertexList.size() - 1]->m_Texture = m_MeshList[m_MeshList.size() - 1]->m_VertexList[tempInt]->m_Texture;

								m_MeshList[m_MeshList.size() - 1]->m_VertexList[m_MeshList[m_MeshList.size() - 1]->m_VertexList.size() - 1]->isVertexSet = true;

								// ���� �������� ���ؽ��� ���� ���ø��� ���ؽ��� �ε��� ����Ʈ�� ������ �ֱ� ����
								m_MeshList[m_MeshList.size() - 1]->m_VertexList[tempInt]->m_splitList.push_back((unsigned int)m_MeshList[m_MeshList.size() - 1]->m_VertexList.size() - 1);
							}
						}
					}
				}

				// ���̽��� �̷�� ���ؽ��� ��ȸ
				for (int i = 0; i < 3; i++)
				{
					// ���̽� ����Ʈ�� �ε����� ���� �о���� *MESH_VERTEXNORMAL �ε����� ��ġ�� ���
					if (m_MeshList[m_MeshList.size() - 1]->m_FaceList[faceIndex]->m_indexList[i] == tempInt)
					{
						// �� ���̽� �ε����� ���ؽ��� ���� �������� ���� ���ؽ����� ��ȸ
						for (int j = 1; j < m_MeshList[m_MeshList.size() - 1]->m_VertexList[tempInt]->m_splitList.size(); j++)
						{
							// *MESH_VERTEXNORMAL �븻 ���� ��ġ�ϴ� ���ؽ��� �����Ѵٸ�
							if (m_MeshList[m_MeshList.size() - 1]->m_VertexList[m_MeshList[m_MeshList.size() - 1]->m_VertexList[tempInt]->m_splitList[j]]->m_Normal.x == tempVec3.x &&
								m_MeshList[m_MeshList.size() - 1]->m_VertexList[m_MeshList[m_MeshList.size() - 1]->m_VertexList[tempInt]->m_splitList[j]]->m_Normal.y == tempVec3.z &&
								m_MeshList[m_MeshList.size() - 1]->m_VertexList[m_MeshList[m_MeshList.size() - 1]->m_VertexList[tempInt]->m_splitList[j]]->m_Normal.z == tempVec3.y)
							{
								// �� ���ؽ��� ���̽� ����Ʈ�� �ε����� ��ü
								m_MeshList[m_MeshList.size() - 1]->m_FaceList[faceIndex]->m_indexList[i] = m_MeshList[m_MeshList.size() - 1]->m_VertexList[tempInt]->m_splitList[j];
								break;
							}
						}
					}
				}

				tempBool = false;
				tempInt = 0;
				break;
			case TOKENR_MESH_NUMBONE:
				m_BoneExist = true;
				m_MeshList[m_MeshList.size() - 1]->m_TotalBoneNum = Parsing_NumberInt();
				break;

			case TOKENR_BONE_NAME:
				tempString = Parsing_String();
				m_MeshList[m_MeshList.size() - 1]->m_BoneNameList.push_back(tempString);
				tempString.clear();
				break;

			case TOKENR_MESH_WEIGHT:
				m_MeshList[m_MeshList.size() - 1]->m_TempBoneIndex = Parsing_NumberInt();
				break;

			case TOKENR_BONE_BLENGING_WEIGHT:
				tempInt = Parsing_NumberInt();
				m_MeshList[m_MeshList.size() - 1]->m_VertexList[m_MeshList[m_MeshList.size() - 1]->m_TempBoneIndex]->m_BoneWeightList.emplace_back(tempInt, Parsing_NumberFloat());

				// ���⼭ ���ؽ� ���ø��Ǿ� �߰��� ���ؽ��� weight ������ �����ؼ� �־���
				if (m_MeshList[m_MeshList.size() - 1]->m_VertexList[m_MeshList[m_MeshList.size() - 1]->m_TempBoneIndex]->m_splitList.size() > 1)
				{
					for (int i = 1; i < m_MeshList[m_MeshList.size() - 1]->m_VertexList[m_MeshList[m_MeshList.size() - 1]->m_TempBoneIndex]->m_splitList.size(); i++)
					{
						tempInt = m_MeshList[m_MeshList.size() - 1]->m_VertexList[m_MeshList[m_MeshList.size() - 1]->m_TempBoneIndex]->m_splitList[i];
						m_MeshList[m_MeshList.size() - 1]->m_VertexList[tempInt]->m_BoneWeightList = m_MeshList[m_MeshList.size() - 1]->m_VertexList[m_MeshList[m_MeshList.size() - 1]->m_TempBoneIndex]->m_BoneWeightList;
					}
				}

				tempInt = 0;
				break;

			case TOKEND_END:
				return;
				break;

			default:
				break;
		}
	}
}

void ASEParser::VertexSplit()
{
	int faceIndex = 0;
	int splitSize = 0;
	int tempInt3 = 0;
	/// ���ؽ� �븻���� ���̽� �븻������ ���߾��µ� ��Ű�� �Ǹ������� ���ؽ� �븻���� ���̽� �븻���� �ƿ� �ٸ� ��찡 ����
	/// �̷��� �ϸ� �ȵǱ⿡ TOKENR_MESH_VERTEXNORMAL �ܰ迡�� �̸� ��ü

	// ���̽��� ��ȸ�ϸ鼭 
	for (int m = 0; m < m_MeshList.size(); m++)
	{
		//for (int i = 0; i < m_MeshList[m]->m_FaceList.size(); i++)
		//{
		//	for (int j = 0; j < 3; j++)
		//	{
		//		// ���̽��� �̷�� �� ���ؽ� �ε���
		//		faceIndex = m_MeshList[m]->m_FaceList[i]->m_indexList[j];
		//		// ���̽��� �̷�� �� ���ؽ��� ���� ���ø� ������
		//		splitSize = (unsigned int)m_MeshList[m]->m_VertexList[faceIndex]->m_splitList.size();
		//		for (int k = 0; k < splitSize; k++)
		//		{
		//			// ���ø��� ���ؽ����� ��ȸ�ϸ�
		//			tempInt3 = (unsigned int)m_MeshList[m]->m_VertexList[faceIndex]->m_splitList[k];
		//			// ���̽��� �븻���� ���� ���ؽ��� ���̽��� �̷�� ���ؽ��� �ε����� ��ü����
		//			if (m_MeshList[m]->m_FaceList[i]->m_Normal == m_MeshList[m]->m_VertexList[tempInt3]->m_Normal)
		//			{
		//				m_MeshList[m]->m_FaceList[i]->m_indexList[j] = tempInt3;
		//				break;
		//			}
		//		}
		//	}
		//}

		// ���ø� ������� ���� �ʿ�����Ƿ� �Ҵ�� �޸𸮸� ��������
		for (int i = 0; i < m_MeshList[m]->m_VertexList.size(); i++)
		{
			// �� ���� ����
			std::vector<unsigned int> swapVector;
			// �� ���ͷ� �����Ͽ� ���� �޸� ����
			m_MeshList[m]->m_VertexList[i]->m_splitList.swap(swapVector);
		}
	}
}

void ASEParser::ApplyTextureNormal()
{
	// �ؽ��İ� ������ ��쿡�� �۵�
	int faceIndex = 0;
	int tFaceIndex = 0;

	int startInt = 0;
	bool tempBool = false;
	bool findRedundancy = false;

	for (int i = 0; i < m_MeshList.size(); i++)
	{
		if (m_MeshList[i]->m_TvertList.size() != 0)
		{
			for (int j = 0; j < m_MeshList[i]->m_FaceList.size(); j++)
			{
				for (int k = 0; k < 3; k++)
				{
					// ���̽��� ����Ű�� ���ؽ� �ε���
					faceIndex = m_MeshList[i]->m_FaceList[j]->m_indexList[k];
					// T���̽��� ����Ű�� ���ؽ� �ε���
					tFaceIndex = m_MeshList[i]->m_TfaceList[j]->m_indexList[k];

					// ���̽��� ����Ű�� ���ؽ� �ε����� �ؽ��� = T���̽��� ����Ű�� ���ؽ� �ε����� �ؽ���
					// �ؽ��� ���� ����ִٸ� �ؽ��� ���� ����
					if (m_MeshList[i]->m_VertexList[faceIndex]->m_Texture.x == 0 && m_MeshList[i]->m_VertexList[faceIndex]->m_Texture.y == 0)
					{
						m_MeshList[i]->m_VertexList[faceIndex]->m_Texture = m_MeshList[i]->m_TvertList[tFaceIndex]->m_UVW;

					}

					// �ؽ��� ���� �̹� ���ִٸ�, �� pos, norm�� ���� tex�� �ٸ� ���ؽ��� �����Ѵٸ�
					if (m_MeshList[i]->m_VertexList[faceIndex]->m_Texture.x != m_MeshList[i]->m_TvertList[tFaceIndex]->m_UVW.x || m_MeshList[i]->m_VertexList[faceIndex]->m_Texture.y != m_MeshList[i]->m_TvertList[tFaceIndex]->m_UVW.y)
					{
						if (tempBool == false)
						{
							// �ؽ����ʿ��� ���ؽ� ���ø� �ϱ� �� ���� ���ؽ� ����Ʈ�� ũ�⸦ ����
							startInt = (int)(m_MeshList[i]->m_VertexList.size() - 1);
							tempBool = true;
						}

						ASE_Parser::Vertex tempVertex;
						tempVertex.m_Position = m_MeshList[i]->m_VertexList[faceIndex]->m_Position;
						tempVertex.m_Normal = m_MeshList[i]->m_VertexList[faceIndex]->m_Normal;
						tempVertex.m_BoneWeightList = m_MeshList[i]->m_VertexList[faceIndex]->m_BoneWeightList;
						tempVertex.m_Texture = m_MeshList[i]->m_TvertList[tFaceIndex]->m_UVW;

						// �ؽ����ʿ��� ���ؽ� ���ø��� �� �� ���ܳ� ���ؽ����� ���ؽ� ����Ʈ�� ��ȸ
						for (int m = startInt; m < m_MeshList[i]->m_VertexList.size(); m++)
						{
							// �ؽ����ʿ��� ���� ���ܳ� ���ؽ��� �ߺ��� ���ؽ��� �ִ��� üũ 
							if (m_MeshList[i]->m_VertexList[m]->m_Position == tempVertex.m_Position &&
								m_MeshList[i]->m_VertexList[m]->m_Normal == tempVertex.m_Normal &&
								m_MeshList[i]->m_VertexList[m]->m_Texture == tempVertex.m_Texture)
							{
								findRedundancy = true;
								// �ߺ��� ���ؽ��� �ִٸ� �� �ߺ��� ���ؽ��� ���̽� �ε����� ��ü����
								m_MeshList[i]->m_FaceList[j]->m_indexList[k] = m;
								break;
							}
						}

						// �ߺ��� ���ؽ��� ���ٸ�
						if (findRedundancy == false)
						{
							// ���ؽ� �߰�
							CreateOneVertexToList(i);

							// pos, normal ���� �����ؿ�
							m_MeshList[i]->m_VertexList[m_MeshList[i]->m_VertexList.size() - 1]->m_Position = tempVertex.m_Position;
							m_MeshList[i]->m_VertexList[m_MeshList[i]->m_VertexList.size() - 1]->m_Normal = tempVertex.m_Normal;
							m_MeshList[i]->m_VertexList[m_MeshList[i]->m_VertexList.size() - 1]->m_BoneWeightList = tempVertex.m_BoneWeightList;

							// texture ���� ���ο� ������ ä��
							m_MeshList[i]->m_VertexList[m_MeshList[i]->m_VertexList.size() - 1]->m_Texture = tempVertex.m_Texture;

							// ���̽��� ����ִ� ���ؽ� �ε����� ���� ���� ������ ���ؽ��� ��ü����
							m_MeshList[i]->m_FaceList[j]->m_indexList[k] = (unsigned int)(m_MeshList[i]->m_VertexList.size() - 1);
						}
					}

					findRedundancy = false;
				}
			}
		}
	}
}

void ASEParser::SetRelationShip(const eObjectState eState, ASE_Parser::ASEOBJECT* baseObject, ASE_Parser::ASEOBJECT* pASEOBJECT, const std::string& parentName)
{
	switch (eState)
	{
		/// GEOMOBJECT�� �θ�� GEOM�̳� HELPER
		case eObjectState::GEOMOBJECT:
			// GEOM (MESH)�� �θ� �����Ѵٸ�
			for (int i = 0; i < m_MeshList.size(); i++)
			{
				// ��ġ�� ����� �̸��� �θ��� ��� �̸��� ���ٸ�
				if (m_MeshList[i]->m_NodeName == parentName)
				{
					// �θ��� ����Ʈ�� �߰�
					baseObject->_parentObject.push_back((ASE_Parser::ASEOBJECT*)m_MeshList[i]);
					m_MeshList[i]->_childObject.push_back(baseObject);
					// �θ��� �θ� ��� Ÿ�� ���鼭 �θ��� ����Ʈ�� �߰���Ŵ
					SetRelationShip(baseObject, (ASE_Parser::ASEOBJECT*)m_MeshList[i]);
					break;
				}
			}

			// Helper�� �θ� �����Ѵٸ�
			for (int i = 0; i < m_HelperList.size(); i++)
			{
				// ��ġ�� ����� �̸��� �θ��� ��� �̸��� ���ٸ�
				if (m_HelperList[i]->m_NodeName == parentName)
				{
					// �θ��� ����Ʈ�� �߰�
					baseObject->_parentObject.push_back((ASE_Parser::ASEOBJECT*)m_HelperList[i]);
					m_HelperList[i]->_childObject.push_back(baseObject);
					// �θ��� �θ� ��� Ÿ�� ���鼭 �θ��� ����Ʈ�� �߰���Ŵ
					SetRelationShip(baseObject, (ASE_Parser::ASEOBJECT*)m_HelperList[i]);
					break;
				}
			}
			break;

			/// HELPER�� �θ�� HELPER�� SHAPE
		case eObjectState::HELPEROBEJCT:
			// Helper�� �θ� �����Ѵٸ�
			for (int i = 0; i < m_HelperList.size(); i++)
			{
				// ��ġ�� ����� �̸��� �θ��� ��� �̸��� ���ٸ�
				if (m_HelperList[i]->m_NodeName == parentName)
				{
					// �θ��� ����Ʈ�� �߰�
					baseObject->_parentObject.push_back((ASE_Parser::ASEOBJECT*)m_HelperList[i]);
					m_HelperList[i]->_childObject.push_back(baseObject);
					// �θ��� �θ� ��� Ÿ�� ���鼭 �θ��� ����Ʈ�� �߰���Ŵ
					SetRelationShip(baseObject, (ASE_Parser::ASEOBJECT*)m_HelperList[i]);
					break;
				}
			}

			// Shape�� �θ� �����Ѵٸ�
			for (int i = 0; i < m_ShapeList.size(); i++)
			{
				// ��ġ�� ����� �̸��� �θ��� ��� �̸��� ���ٸ�
				if (m_ShapeList[i]->m_NodeName == parentName)
				{
					// �θ��� ����Ʈ�� �߰�
					baseObject->_parentObject.push_back((ASE_Parser::ASEOBJECT*)m_ShapeList[i]);
					m_ShapeList[i]->_childObject.push_back(baseObject);
					// �θ��� �θ� ��� Ÿ�� ���鼭 �θ��� ����Ʈ�� �߰���Ŵ
					SetRelationShip(baseObject, (ASE_Parser::ASEOBJECT*)m_ShapeList[i]);
					break;
				}
			}
			break;

			/// SHAPE�� �θ�� SHAPE, GEOMOBJECT(?)
		case eObjectState::SHAPEOBJECT:
			// Shape�� �θ� �����Ѵٸ�
			for (int i = 0; i < m_ShapeList.size(); i++)
			{
				// ��ġ�� ����� �̸��� �θ��� ��� �̸��� ���ٸ�
				if (m_ShapeList[i]->m_NodeName == parentName)
				{
					// �θ��� ����Ʈ�� �߰�
					baseObject->_parentObject.push_back((ASE_Parser::ASEOBJECT*)m_ShapeList[i]);
					m_ShapeList[i]->_childObject.push_back(baseObject);
					// �θ��� �θ� ��� Ÿ�� ���鼭 �θ��� ����Ʈ�� �߰���Ŵ
					SetRelationShip(baseObject, (ASE_Parser::ASEOBJECT*)m_ShapeList[i]);
					break;
				}
			}

			// GEOM�� �θ� �����Ѵٸ�
			for (int i = 0; i < m_MeshList.size(); i++)
			{
				// ��ġ�� ����� �̸��� �θ��� ��� �̸��� ���ٸ�
				if (m_MeshList[i]->m_NodeName == parentName)
				{
					// �θ��� ����Ʈ�� �߰�
					baseObject->_parentObject.push_back((ASE_Parser::ASEOBJECT*)m_MeshList[i]);
					m_MeshList[i]->_childObject.push_back(baseObject);
					// �θ��� �θ� ��� Ÿ�� ���鼭 �θ��� ����Ʈ�� �߰���Ŵ
					SetRelationShip(baseObject, (ASE_Parser::ASEOBJECT*)m_MeshList[i]);
					break;
				}
			}
			break;
	}
}

void ASEParser::SetRelationShip(ASE_Parser::ASEOBJECT* baseObject, ASE_Parser::ASEOBJECT* pASEOBJECT)
{
	for (int i = 0; i < pASEOBJECT->_parentObject.size(); i++)
	{
		baseObject->_parentObject.push_back((ASE_Parser::ASEOBJECT*)pASEOBJECT->_parentObject[i]);
	}
}

void ASEParser::CreateOneMeshToList()
{
	ASE_Parser::Mesh* pMesh = new ASE_Parser::Mesh();
	m_MeshList.push_back(pMesh);
}

void ASEParser::CreateOneVertexToList()
{
	ASE_Parser::Vertex* pVertex = new ASE_Parser::Vertex();
	m_MeshList[m_MeshList.size() - 1]->m_VertexList.push_back(pVertex);
	m_MeshList[m_MeshList.size() - 1]->m_TotalVertexNum++;
}

void ASEParser::CreateOneVertexToList(int index)
{
	ASE_Parser::Vertex* pVertex = new ASE_Parser::Vertex();
	m_MeshList[index]->m_VertexList.push_back(pVertex);
	m_MeshList[index]->m_TotalVertexNum++;
}

void ASEParser::CreateOneFaceToList()
{
	ASE_Parser::Face* pFace = new ASE_Parser::Face();
	m_MeshList[m_MeshList.size() - 1]->m_FaceList.push_back(pFace);
}

void ASEParser::CreateOneTvertToList()
{
	ASE_Parser::Tvert* pTvert = new ASE_Parser::Tvert();
	m_MeshList[m_MeshList.size() - 1]->m_TvertList.push_back(pTvert);
}

void ASEParser::CreateOneTfaceToList()
{
	ASE_Parser::Tface* pTface = new ASE_Parser::Tface();
	m_MeshList[m_MeshList.size() - 1]->m_TfaceList.push_back(pTface);
}

void ASEParser::CreateOneHelperToList()
{
	ASE_Parser::Helper* pHelper = new ASE_Parser::Helper();
	m_HelperList.push_back(pHelper);
}

void ASEParser::CreateOneShapeToList()
{
	ASE_Parser::Shape* pShape = new ASE_Parser::Shape();
	m_ShapeList.push_back(pShape);
}

void ASEParser::SetOriginalWorldTM()
{
	// row2�� row3�� �ٲٸ� y�� z�� ��ġ�� �ٲ�� ��
	for (int i = 0; i < m_MeshList.size(); i++)
	{
		m_MeshList[i]->_originWorldTM._11 = m_MeshList[i]->m_Tm_Row0.x;
		m_MeshList[i]->_originWorldTM._12 = m_MeshList[i]->m_Tm_Row0.z;
		m_MeshList[i]->_originWorldTM._13 = m_MeshList[i]->m_Tm_Row0.y;
		m_MeshList[i]->_originWorldTM._14 = 0;

		m_MeshList[i]->_originWorldTM._21 = m_MeshList[i]->m_Tm_Row2.x;
		m_MeshList[i]->_originWorldTM._22 = m_MeshList[i]->m_Tm_Row2.z;
		m_MeshList[i]->_originWorldTM._23 = m_MeshList[i]->m_Tm_Row2.y;
		m_MeshList[i]->_originWorldTM._24 = 0;

		m_MeshList[i]->_originWorldTM._31 = m_MeshList[i]->m_Tm_Row1.x;
		m_MeshList[i]->_originWorldTM._32 = m_MeshList[i]->m_Tm_Row1.z;
		m_MeshList[i]->_originWorldTM._33 = m_MeshList[i]->m_Tm_Row1.y;
		m_MeshList[i]->_originWorldTM._34 = 0;

		m_MeshList[i]->_originWorldTM._41 = m_MeshList[i]->m_Tm_Row3.x;
		m_MeshList[i]->_originWorldTM._42 = m_MeshList[i]->m_Tm_Row3.z;
		m_MeshList[i]->_originWorldTM._43 = m_MeshList[i]->m_Tm_Row3.y;
		m_MeshList[i]->_originWorldTM._44 = 1.0f;

		// �θ� ���� ������Ʈ�̸鼭 Bone�� �ƴ� ���
		if (m_MeshList[i]->_parentObject.empty())
		{
			// �� ������Ʈ�� LocalTM�� WolrdTM�� ����
			m_MeshList[i]->_originLocalTM = m_MeshList[i]->_originWorldTM;
		}
	}

	for (int i = 0; i < m_HelperList.size(); i++)
	{
		m_HelperList[i]->_originWorldTM._11 = m_HelperList[i]->m_Tm_Row0.x;
		m_HelperList[i]->_originWorldTM._12 = m_HelperList[i]->m_Tm_Row0.z;
		m_HelperList[i]->_originWorldTM._13 = m_HelperList[i]->m_Tm_Row0.y;
		m_HelperList[i]->_originWorldTM._14 = 0;

		m_HelperList[i]->_originWorldTM._21 = m_HelperList[i]->m_Tm_Row2.x;
		m_HelperList[i]->_originWorldTM._22 = m_HelperList[i]->m_Tm_Row2.z;
		m_HelperList[i]->_originWorldTM._23 = m_HelperList[i]->m_Tm_Row2.y;
		m_HelperList[i]->_originWorldTM._24 = 0;

		m_HelperList[i]->_originWorldTM._31 = m_HelperList[i]->m_Tm_Row1.x;
		m_HelperList[i]->_originWorldTM._32 = m_HelperList[i]->m_Tm_Row1.z;
		m_HelperList[i]->_originWorldTM._33 = m_HelperList[i]->m_Tm_Row1.y;
		m_HelperList[i]->_originWorldTM._34 = 0;

		m_HelperList[i]->_originWorldTM._41 = m_HelperList[i]->m_Tm_Row3.x;
		m_HelperList[i]->_originWorldTM._42 = m_HelperList[i]->m_Tm_Row3.z;
		m_HelperList[i]->_originWorldTM._43 = m_HelperList[i]->m_Tm_Row3.y;
		m_HelperList[i]->_originWorldTM._44 = 1.0f;

		// �θ� ���� ������Ʈ���
		if (m_HelperList[i]->_parentObject.empty())
		{
			// �� ������Ʈ�� LocalTM�� WolrdTM�� ����
			m_HelperList[i]->_originLocalTM = m_HelperList[i]->_originWorldTM;
		}
	}

	for (int i = 0; i < m_ShapeList.size(); i++)
	{
		m_ShapeList[i]->_originWorldTM._11 = m_ShapeList[i]->m_Tm_Row0.x;
		m_ShapeList[i]->_originWorldTM._12 = m_ShapeList[i]->m_Tm_Row0.z;
		m_ShapeList[i]->_originWorldTM._13 = m_ShapeList[i]->m_Tm_Row0.y;
		m_ShapeList[i]->_originWorldTM._14 = 0;

		m_ShapeList[i]->_originWorldTM._21 = m_ShapeList[i]->m_Tm_Row2.x;
		m_ShapeList[i]->_originWorldTM._22 = m_ShapeList[i]->m_Tm_Row2.z;
		m_ShapeList[i]->_originWorldTM._23 = m_ShapeList[i]->m_Tm_Row2.y;
		m_ShapeList[i]->_originWorldTM._24 = 0;

		m_ShapeList[i]->_originWorldTM._31 = m_ShapeList[i]->m_Tm_Row1.x;
		m_ShapeList[i]->_originWorldTM._32 = m_ShapeList[i]->m_Tm_Row1.z;
		m_ShapeList[i]->_originWorldTM._33 = m_ShapeList[i]->m_Tm_Row1.y;
		m_ShapeList[i]->_originWorldTM._34 = 0;

		m_ShapeList[i]->_originWorldTM._41 = m_ShapeList[i]->m_Tm_Row3.x;
		m_ShapeList[i]->_originWorldTM._42 = m_ShapeList[i]->m_Tm_Row3.z;
		m_ShapeList[i]->_originWorldTM._43 = m_ShapeList[i]->m_Tm_Row3.y;
		m_ShapeList[i]->_originWorldTM._44 = 1.0f;

		// �θ� ���� ������Ʈ���
		if (m_ShapeList[i]->_parentObject.empty())
		{
			// �� ������Ʈ�� LocalTM�� WolrdTM�� ����
			m_ShapeList[i]->_originLocalTM = m_ShapeList[i]->_originWorldTM;
		}
	}
}

void ASEParser::SetLocalTM()
{
	for (int i = 0; i < m_MeshList.size(); i++)
	{
		if (m_MeshList[i]->_parentObject.empty() == false)
		{
			DirectX::XMMATRIX myWorld = DirectX::XMLoadFloat4x4(&m_MeshList[i]->_originWorldTM);
			// �θ� �����ϴ� ������Ʈ��� �θ� ��ȸ�ϸ� �θ��� ������� ���� �� �θ𿡼����� ������
			for (int j = (int)m_MeshList[i]->_parentObject.size() - 1; j >= 0; j--)
			{
				// �θ��� Local����� ������
				DirectX::XMMATRIX parentLocal = DirectX::XMLoadFloat4x4(&m_MeshList[i]->_parentObject[j]->_originLocalTM);
				// �θ��� Local ����� �����
				DirectX::XMMATRIX parentLocalInverse = DirectX::XMMatrixInverse(nullptr, parentLocal);
				// �� World���� �θ��� Local�� ������� ������
				myWorld = DirectX::XMMatrixMultiply(myWorld, parentLocalInverse);
			}
			// ��� ���� �� LocalTM�� ����
			DirectX::XMStoreFloat4x4(&m_MeshList[i]->_originLocalTM, myWorld);
		}
	}

	for (int i = 0; i < m_HelperList.size(); i++)
	{
		if (m_HelperList[i]->_parentObject.empty() == false)
		{
			DirectX::XMMATRIX myWorld = DirectX::XMLoadFloat4x4(&m_HelperList[i]->_originWorldTM);
			// �θ� �����ϴ� ������Ʈ��� �θ� ��ȸ�ϸ� �θ��� ������� ���� �� �θ𿡼����� ������
			for (int j = (int)m_HelperList[i]->_parentObject.size() - 1; j >= 0; j--)
			{
				// �θ��� Local����� ������
				DirectX::XMMATRIX parentLocal = DirectX::XMLoadFloat4x4(&m_HelperList[i]->_parentObject[j]->_originLocalTM);
				// �θ��� Local ����� �����
				DirectX::XMMATRIX parentLocalInverse = DirectX::XMMatrixInverse(nullptr, parentLocal);
				// �� World���� �θ��� Local�� ������� ������
				myWorld = DirectX::XMMatrixMultiply(myWorld, parentLocalInverse);
			}
			// ��� ���� �� LocalTM�� ����
			DirectX::XMStoreFloat4x4(&m_HelperList[i]->_originLocalTM, myWorld);
		}
	}

	for (int i = 0; i < m_ShapeList.size(); i++)
	{
		if (m_ShapeList[i]->_parentObject.empty() == false)
		{
			DirectX::XMMATRIX myWorld = DirectX::XMLoadFloat4x4(&m_ShapeList[i]->_originWorldTM);
			// �θ� �����ϴ� ������Ʈ��� �θ� ��ȸ�ϸ� �θ��� ������� ���� �� �θ𿡼����� ������
			for (int j = (int)m_ShapeList[i]->_parentObject.size() - 1; j >= 0; j--)
			{
				// �θ��� Local����� ������
				DirectX::XMMATRIX parentLocal = DirectX::XMLoadFloat4x4(&m_ShapeList[i]->_parentObject[j]->_originLocalTM);
				// �θ��� Local ����� �����
				DirectX::XMMATRIX parentLocalInverse = DirectX::XMMatrixInverse(nullptr, parentLocal);
				// �� World���� �θ��� Local�� ������� ������
				myWorld = DirectX::XMMatrixMultiply(myWorld, parentLocalInverse);
			}
			// ��� ���� �� LocalTM�� ����
			DirectX::XMStoreFloat4x4(&m_ShapeList[i]->_originLocalTM, myWorld);
		}
	}
}

void ASEParser::SetNewWorldTM()
{
	for (int i = 0; i < m_MeshList.size(); i++)
	{
		// �� ���ú��� �θ��� ������ �����ֱ����� �ʱ� ���� ������ �� ������ ��� ����
		m_MeshList[i]->_newWorldTM.at(_meshCount) = m_MeshList[i]->_originLocalTM;

		if (m_MeshList[i]->_parentObject.empty() == false)
		{
			for (int j = 0; j < m_MeshList[i]->_parentObject.size(); j++)

			{
				// �� Local�� �θ��� Local ����� ������
				DirectX::XMMATRIX myWorld = DirectX::XMLoadFloat4x4(&m_MeshList[i]->_newWorldTM.at(_meshCount));
				DirectX::XMMATRIX parentLocal = DirectX::XMLoadFloat4x4(&m_MeshList[i]->_parentObject[j]->_originLocalTM);
				// �� Local�� �θ��� Local�� ���� ����� NewWorld�� ����
				DirectX::XMStoreFloat4x4(&m_MeshList[i]->_newWorldTM.at(_meshCount), DirectX::XMMatrixMultiply(myWorld, parentLocal));
			}
		}
	}

	for (int i = 0; i < m_HelperList.size(); i++)
	{
		// �� ���ú��� �θ��� ������ �����ֱ����� �ʱ� ���� ������ �� ������ ��� ����
		m_HelperList[i]->_newWorldTM.at(_meshCount) = m_HelperList[i]->_originLocalTM;

		if (m_HelperList[i]->_parentObject.empty() == false)
		{
			for (int j = 0; j < m_HelperList[i]->_parentObject.size(); j++)
			{
				// �� Local�� �θ��� Local ����� ������
				DirectX::XMMATRIX myWorld = DirectX::XMLoadFloat4x4(&m_HelperList[i]->_newWorldTM.at(_meshCount));
				DirectX::XMMATRIX parentLocal = DirectX::XMLoadFloat4x4(&m_HelperList[i]->_parentObject[j]->_originLocalTM);
				// �� Local�� �θ��� Local�� ���� ����� NewWorld�� ����
				DirectX::XMStoreFloat4x4(&m_HelperList[i]->_newWorldTM.at(_meshCount), DirectX::XMMatrixMultiply(myWorld, parentLocal));
			}
		}
	}

	for (int i = 0; i < m_ShapeList.size(); i++)
	{
		// �� ���ú��� �θ��� ������ �����ֱ����� �ʱ� ���� ������ �� ������ ��� ����
		m_ShapeList[i]->_newWorldTM.at(_meshCount) = m_ShapeList[i]->_originLocalTM;

		if (m_ShapeList[i]->_parentObject.empty() == false)
		{
			for (int j = 0; j < m_ShapeList[i]->_parentObject.size(); j++)
			{
				// �� Local�� �θ��� Local ����� ������
				DirectX::XMMATRIX myWorld = DirectX::XMLoadFloat4x4(&m_ShapeList[i]->_newWorldTM.at(_meshCount));
				DirectX::XMMATRIX parentLocal = DirectX::XMLoadFloat4x4(&m_ShapeList[i]->_parentObject[j]->_originLocalTM);
				// �� Local�� �θ��� Local�� ���� ����� NewWorld�� ����
				DirectX::XMStoreFloat4x4(&m_ShapeList[i]->_newWorldTM.at(_meshCount), DirectX::XMMatrixMultiply(myWorld, parentLocal));
			}
		}
	}
}


void ASEParser::SetSpecificNewWorldTM(ASE_Parser::ASEOBJECT* pMesh, int meshIndex)
{
	// ����TM�� �ٲ� �����̹Ƿ� ����TM�� �ٽ� NewWorldTM�� ������
	DirectX::XMMATRIX myLocal = DirectX::XMLoadFloat4x4(&pMesh->_localTM.at(meshIndex));
	DirectX::XMStoreFloat4x4(&pMesh->_newWorldTM.at(meshIndex), myLocal);

	for (int i = 0; i < pMesh->_parentObject.size(); i++)
	{
		// �θ��� ����TM�� ���
		DirectX::XMMATRIX myWorld = DirectX::XMLoadFloat4x4(&pMesh->_newWorldTM.at(meshIndex));
		DirectX::XMMATRIX parentLocal = DirectX::XMLoadFloat4x4(&pMesh->_parentObject[i]->_localTM.at(meshIndex));
		// �� Local�� �θ��� Local�� ���� ����� NewWorld�� ����
		DirectX::XMStoreFloat4x4(&pMesh->_newWorldTM.at(meshIndex), DirectX::XMMatrixMultiply(myWorld, parentLocal));
	}

	while (1)
	{
		// �ڽ��� ��� Ÿ���� �ڽ��� ���ٸ� Ż��
		if (pMesh->_childObject.empty())
		{
			break;
		}
		else
		{
			for (int i = 0; i < pMesh->_childObject.size(); i++)
			{
				DirectX::XMMATRIX myLocal = DirectX::XMLoadFloat4x4(&pMesh->_childObject[i]->_localTM.at(meshIndex));
				DirectX::XMStoreFloat4x4(&pMesh->_childObject[i]->_newWorldTM.at(meshIndex), myLocal);

				for (int j = 0; j < pMesh->_childObject[i]->_parentObject.size(); j++)
				{
					DirectX::XMMATRIX myWorld = DirectX::XMLoadFloat4x4(&pMesh->_childObject[i]->_newWorldTM.at(meshIndex));
					DirectX::XMMATRIX parentLocal = DirectX::XMLoadFloat4x4(&pMesh->_childObject[i]->_parentObject[j]->_localTM.at(meshIndex));
					DirectX::XMStoreFloat4x4(&pMesh->_childObject[i]->_newWorldTM.at(meshIndex), DirectX::XMMatrixMultiply(myWorld, parentLocal));
				}
				SetSpecificNewWorldTM(pMesh->_childObject[i], meshIndex);
			}
			break;
		}
	}
}

void ASEParser::SetSpecificSlerpTM(ASE_Parser::ASEOBJECT* pMesh, int meshIndex)
{
	// ����TM�� �ٲ� �����̹Ƿ� ����TM�� �ٽ� NewWorldTM�� ������
	DirectX::XMMATRIX mySlerp = DirectX::XMLoadFloat4x4(&pMesh->_slerpTM.at(meshIndex));
	DirectX::XMStoreFloat4x4(&pMesh->_newWorldTM.at(meshIndex), mySlerp);

	for (int i = 0; i < pMesh->_parentObject.size(); i++)
	{
		// �θ��� ����TM�� ���
		DirectX::XMMATRIX myWorld = DirectX::XMLoadFloat4x4(&pMesh->_newWorldTM.at(meshIndex));
		DirectX::XMMATRIX parentLocal = DirectX::XMLoadFloat4x4(&pMesh->_parentObject[i]->_slerpTM.at(meshIndex));
		// �� Local�� �θ��� Local�� ���� ����� NewWorld�� ����
		DirectX::XMStoreFloat4x4(&pMesh->_newWorldTM.at(meshIndex), DirectX::XMMatrixMultiply(myWorld, parentLocal));
	}

	while (1)
	{
		// �ڽ��� ��� Ÿ���� �ڽ��� ���ٸ� Ż��
		if (pMesh->_childObject.empty())
		{
			break;
		}
		else
		{
			for (int i = 0; i < pMesh->_childObject.size(); i++)
			{
				DirectX::XMMATRIX mySlerp = DirectX::XMLoadFloat4x4(&pMesh->_childObject[i]->_slerpTM.at(meshIndex));
				DirectX::XMStoreFloat4x4(&pMesh->_childObject[i]->_newWorldTM.at(meshIndex), mySlerp);

				for (int j = 0; j < pMesh->_childObject[i]->_parentObject.size(); j++)
				{
					DirectX::XMMATRIX myWorld = DirectX::XMLoadFloat4x4(&pMesh->_childObject[i]->_newWorldTM.at(meshIndex));
					DirectX::XMMATRIX parentLocal = DirectX::XMLoadFloat4x4(&pMesh->_childObject[i]->_parentObject[j]->_slerpTM.at(meshIndex));
					DirectX::XMStoreFloat4x4(&pMesh->_childObject[i]->_newWorldTM.at(meshIndex), DirectX::XMMatrixMultiply(myWorld, parentLocal));
				}
				SetSpecificSlerpTM(pMesh->_childObject[i], meshIndex);
			}
			break;
		}
	}
}



void ASEParser::SetToLocalPosition()
{
	for (int i = 0; i < m_MeshList.size(); i++)
	{
		for (int j = 0; j < m_MeshList[i]->m_VertexList.size(); j++)
		{
			// ������ ����� ����
			DirectX::XMMATRIX myWorld = XMLoadFloat4x4(&m_MeshList[i]->_originWorldTM);
			DirectX::XMMATRIX myWorldInverse = XMMatrixInverse(nullptr, myWorld);
			// ������ 
			DirectX::XMVECTOR positon = { m_MeshList[i]->m_VertexList[j]->m_Position.x, m_MeshList[i]->m_VertexList[j]->m_Position.y, m_MeshList[i]->m_VertexList[j]->m_Position.z };

			DirectX::XMVECTOR result = DirectX::XMVector3TransformCoord(positon, myWorldInverse);

			DirectX::XMFLOAT4 tempFloat4;
			DirectX::XMStoreFloat4(&tempFloat4, result);

			m_MeshList[i]->m_VertexList[j]->m_Position.x = tempFloat4.x;
			m_MeshList[i]->m_VertexList[j]->m_Position.y = tempFloat4.y;
			m_MeshList[i]->m_VertexList[j]->m_Position.z = tempFloat4.z;
		}
	}
}

void ASEParser::SetToNewWorldPosition()
{
	for (int i = 0; i < m_MeshList.size(); i++)
	{
		for (int j = 0; j < m_MeshList[i]->m_VertexList.size(); j++)
		{
			DirectX::XMMATRIX myWorld = XMLoadFloat4x4(&m_MeshList[i]->_newWorldTM.at(_meshCount - 1));

			DirectX::XMVECTOR positon = { m_MeshList[i]->m_VertexList[j]->m_Position.x, m_MeshList[i]->m_VertexList[j]->m_Position.y, m_MeshList[i]->m_VertexList[j]->m_Position.z };

			// ������ �����ǿ� ��������� ���ؼ� ���� ��ġ�� ����
			DirectX::XMVECTOR result = DirectX::XMVector3TransformCoord(positon, myWorld);

			DirectX::XMFLOAT4 tempFloat4;
			DirectX::XMStoreFloat4(&tempFloat4, result);

			m_MeshList[i]->m_VertexList[j]->m_Position.x = tempFloat4.x;
			m_MeshList[i]->m_VertexList[j]->m_Position.y = tempFloat4.y;
			m_MeshList[i]->m_VertexList[j]->m_Position.z = tempFloat4.z;
		}
	}
}

void ASEParser::SetBoneList()
{
	if (m_BoneExist == true)
	{
		std::vector<std::string> tempVec;

		for (int i = 0; i < m_MeshList.size(); i++)
		{
			for (int j = 0; j < m_MeshList[i]->m_BoneNameList.size(); j++)
			{
				for (int k = 0; k < m_MeshList.size(); k++)
				{
					if (m_MeshList[k]->m_NodeName == m_MeshList[i]->m_BoneNameList[j])
					{
						m_MeshList[k]->m_isBone = true;
						m_MeshList[i]->m_BoneList.push_back(m_MeshList[k]);

						for (int m = 0; m < m_MeshList[k]->_parentObject.size(); m++)
						{
							// Bone List�� ��õ� ���� �θ���� ���� ������ �ϱ⿡ ������ �� �ʿ䰡 ���⿡ ���� �θ�鵵 ������ �Ǵ�
							// ���̳��� ĳ��Ʈ�� �Ϸ��� �ߴµ� virtual ��Ӱ��谡 �ƴ϶� �Ұ����Ͽ� �̷��� ��
							ASE_Parser::Mesh* parentObject = (ASE_Parser::Mesh*)(m_MeshList[k]->_parentObject[m]);
							parentObject->m_isBone = true;
						}

						// �ڽ��� ��� �ؾ��ϳ�? �θ� ���̸� �ڽĵ� ������ ��?
// 						for (int m = 0; m < m_MeshList[k]->m_ChildObject.size(); m++)
// 						{
// 							ASE_Parser::Mesh* childObject = (ASE_Parser::Mesh*)(m_MeshList[k]->m_ChildObject[m]);
// 							childObject->m_isBone = true;
// 						}
					}
				}
			}
			m_MeshList[i]->m_BoneNameList.swap(tempVec);
		}
	}
}


bool ASEParser::GetIsAnimated()
{
	return m_isAnimated;
}

int ASEParser::GetStartFrame()
{
	return m_StartFrame;
}

int ASEParser::GetFinalFrame()
{
	return m_FinalFrame;
}

bool ASEParser::GetBoneExist()
{
	return m_BoneExist;
}

void ASEParser::AddMeshCount()
{
	_meshCount++;

	for (auto index : m_ShapeList)
	{
		index->_localTM.insert(std::make_pair(_meshCount, index->_originLocalTM));
		index->_newWorldTM.insert(std::make_pair(_meshCount, index->_localTM.at(_meshCount)));
		index->_slerpTM.insert(std::make_pair(_meshCount, index->_localTM.at(_meshCount)));
	}

	for (auto index : m_HelperList)
	{
		index->_localTM.insert(std::make_pair(_meshCount, index->_originLocalTM));
		index->_newWorldTM.insert(std::make_pair(_meshCount, index->_localTM.at(_meshCount)));
		index->_slerpTM.insert(std::make_pair(_meshCount, index->_localTM.at(_meshCount)));
	}

	for (auto index : m_MeshList)
	{
		index->_localTM.insert(std::make_pair(_meshCount, index->_originLocalTM));
		index->_newWorldTM.insert(std::make_pair(_meshCount, index->_localTM.at(_meshCount)));
		index->_slerpTM.insert(std::make_pair(_meshCount, index->_localTM.at(_meshCount)));
	}

	SetToLocalPosition();
	SetNewWorldTM();
}

void ASEParser::DeleteMeshInfo(int meshIndex)
{
	for (auto index : m_ShapeList)
	{
		index->_localTM.erase(meshIndex);
		index->_newWorldTM.erase(meshIndex);
		index->_slerpTM.erase(meshIndex);
	}

	for (auto index : m_HelperList)
	{
		index->_localTM.erase(meshIndex);
		index->_newWorldTM.erase(meshIndex);
		index->_slerpTM.erase(meshIndex);
	}

	for (auto index : m_MeshList)
	{
		index->_localTM.erase(meshIndex);
		index->_newWorldTM.erase(meshIndex);
		index->_slerpTM.erase(meshIndex);
	}
}

int ASEParser::GetMeshCount()
{
	return _meshCount;
}

int ASEParser::Parsing_NumberInt()
{
	LONG token;
	LONG tempNumber;

	token = m_Lexer->GetToken(m_TokenString);
	tempNumber = strtoul(m_TokenString, NULL, 10);

	return tempNumber;
}

float ASEParser::Parsing_NumberFloat()
{
	LONG token;
	float tempNumber;

	token = m_Lexer->GetToken(m_TokenString);
	tempNumber = (float)atof(m_TokenString);

	return tempNumber;
}

DirectX::SimpleMath::Vector3 ASEParser::Parsing_NumberVector3()
{
	LONG token;
	DirectX::SimpleMath::Vector3 tempVector3;

	token = m_Lexer->GetToken(m_TokenString);
	tempVector3.x = (float)atof(m_TokenString);
	token = m_Lexer->GetToken(m_TokenString);
	tempVector3.y = (float)atof(m_TokenString);
	token = m_Lexer->GetToken(m_TokenString);
	tempVector3.z = (float)atof(m_TokenString);

	return tempVector3;
}

DirectX::SimpleMath::Vector4 ASEParser::Parsing_NumberVector4()
{
	LONG token;
	DirectX::SimpleMath::Vector4 tempVector4;

	token = m_Lexer->GetToken(m_TokenString);
	tempVector4.x = (float)atof(m_TokenString);
	token = m_Lexer->GetToken(m_TokenString);
	tempVector4.y = (float)atof(m_TokenString);
	token = m_Lexer->GetToken(m_TokenString);
	tempVector4.z = (float)atof(m_TokenString);
	token = m_Lexer->GetToken(m_TokenString);
	tempVector4.w = (float)atof(m_TokenString);

	return tempVector4;
}

LPSTR ASEParser::Parsing_String()
{
	m_Lexer->GetToken(m_TokenString);

	return m_TokenString;
}

