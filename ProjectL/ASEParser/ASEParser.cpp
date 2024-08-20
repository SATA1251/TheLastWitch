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
		TRACE("파일을 여는 중 문제 발생");
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

				/// GEOMOBJECT, HELPER, SHPAEE 공통의 요소들
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

				// ROW 0~4를 합쳐 행렬을 만들때 전치해주고 여기서는 원형 그대로 받아오도록 함
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

				// 애니메이션을 위한 메쉬 전체에서의 시작 프레임 설정
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

				// 애니메이션을 위한 메쉬 전체에서의 마지막 프레임 설정 
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

				/// GEOMOBJECT만의 요소 - Mesh
			case TOKENR_MESH_NUMVERTEX:
				//m_MeshList[m_MeshList.size() - 1]->m_TotalVertexNum = Parsing_NumberInt();
				break;

			case TOKENR_MESH_NUMFACES:
				m_MeshList[m_MeshList.size() - 1]->m_TotalFaceNum = Parsing_NumberInt();
				break;

			case TOKENR_MESH_VERTEX:
				CreateOneVertexToList();

				tempInt = Parsing_NumberInt();
				// x,z,y 순서로 들어오기에 넣어주는 순서를 바꿔야 함
				m_MeshList[m_MeshList.size() - 1]->m_VertexList[tempInt]->m_Position.x = Parsing_NumberFloat();
				m_MeshList[m_MeshList.size() - 1]->m_VertexList[tempInt]->m_Position.z = Parsing_NumberFloat();
				m_MeshList[m_MeshList.size() - 1]->m_VertexList[tempInt]->m_Position.y = Parsing_NumberFloat();

				tempInt = 0;
				break;

			case TOKENR_MESH_FACE:
				CreateOneFaceToList();

				// 0: 중 :는 버려지고 앞의 숫자부분만 가져옴
				tempInt = Parsing_NumberInt();

				// x,z,y 순서로 들어오기에 넣어주는 순서를 바꿔야 함 -> 0,2,1

				for (int i = 0; i < 3; i++)
				{
					m_MeshList[m_MeshList.size() - 1]->m_FaceList[tempInt]->m_indexList.push_back(0);
				}

				Parsing_String();	// 필요없는 정보 A: 버리기
				m_MeshList[m_MeshList.size() - 1]->m_FaceList[tempInt]->m_indexList[0] = Parsing_NumberInt();
				Parsing_String();	// 필요없는 정보 b: 버리기
				m_MeshList[m_MeshList.size() - 1]->m_FaceList[tempInt]->m_indexList[2] = Parsing_NumberInt();
				Parsing_String();   // 필요없는 정보 C: 버리기
				m_MeshList[m_MeshList.size() - 1]->m_FaceList[tempInt]->m_indexList[1] = Parsing_NumberInt();

				tempInt = 0;
				break;

			case TOKENR_MESH_NUMTVERTEX:
				// 어짜피 페이스 숫자랑 똑같은데 굳이 다른 변수에 담을 필요가 없을거같아서 일단 처리안함
				break;

			case TOKENR_MESH_TVERT:
				CreateOneTvertToList();

				tempInt = Parsing_NumberInt();
				m_MeshList[m_MeshList.size() - 1]->m_TvertList[tempInt]->m_UVW = Parsing_NumberVector3();
				m_MeshList[m_MeshList.size() - 1]->m_TvertList[tempInt]->m_UVW.y *= -1;
				tempInt = 0;
				break;

			case TOKENR_MESH_NUMTVFACES:
				// 어짜피 페이스 숫자랑 똑같은데 굳이 다른 변수에 담을 필요가 없을거같아서 일단 처리안함
				break;

			case TOKENR_MESH_TFACE:
				CreateOneTfaceToList();

				// x,z,y 순서로 들어오기에 넣어주는 순서를 바꿔야 함 -> 0,2,1
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
				// x,z,y 순서로 들어오기에 넣어주는 순서를 바꿔야 함
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
					// x,z,y 순서로 들어오기에 넣어주는 순서를 바꿔야 함
					m_MeshList[m_MeshList.size() - 1]->m_VertexList[tempInt]->m_Normal.x = tempVec3.x;
					m_MeshList[m_MeshList.size() - 1]->m_VertexList[tempInt]->m_Normal.z = tempVec3.y;
					m_MeshList[m_MeshList.size() - 1]->m_VertexList[tempInt]->m_Normal.y = tempVec3.z;
					m_MeshList[m_MeshList.size() - 1]->m_VertexList[tempInt]->isVertexSet = true;

					// 기존 오리지널 버텍스에 현재 스플릿된 버텍스들 인덱스 리스트를 가지고 있기 위함
					m_MeshList[m_MeshList.size() - 1]->m_VertexList[tempInt]->m_splitList.push_back(tempInt);
				}

				// 노말값을 넣으려는데 이미 해당 버텍스에 노말값이 존재하며, 그 노말 값이 현재의 페이스 노말값과 다른 경우
				else if (m_MeshList[m_MeshList.size() - 1]->m_VertexList[tempInt]->isVertexSet == true)
				{
					// 읽어들인 노말 값이 해당 버텍스의 노말 값과 일치하지 않고
					if (m_MeshList[m_MeshList.size() - 1]->m_VertexList[tempInt]->m_Normal.x != tempVec3.x ||
						m_MeshList[m_MeshList.size() - 1]->m_VertexList[tempInt]->m_Normal.y != tempVec3.z ||
						m_MeshList[m_MeshList.size() - 1]->m_VertexList[tempInt]->m_Normal.z != tempVec3.y)
					{
						// 스프릿된 버텍스가 존재하지 않는다면 (스플릿 리스트 내에 자기자신 뿐이라면)
						if (m_MeshList[m_MeshList.size() - 1]->m_VertexList[tempInt]->m_splitList.size() <= 1)
						{
							// 새로운 노말을 담을 버텍스를 추가해야 함
							CreateOneVertexToList();

							// 새로 만든 버텍스에 모든 정보를 옮겨 담음
							m_MeshList[m_MeshList.size() - 1]->m_VertexList[m_MeshList[m_MeshList.size() - 1]->m_VertexList.size() - 1]->m_Position.x = m_MeshList[m_MeshList.size() - 1]->m_VertexList[tempInt]->m_Position.x;
							m_MeshList[m_MeshList.size() - 1]->m_VertexList[m_MeshList[m_MeshList.size() - 1]->m_VertexList.size() - 1]->m_Position.z = m_MeshList[m_MeshList.size() - 1]->m_VertexList[tempInt]->m_Position.z;
							m_MeshList[m_MeshList.size() - 1]->m_VertexList[m_MeshList[m_MeshList.size() - 1]->m_VertexList.size() - 1]->m_Position.y = m_MeshList[m_MeshList.size() - 1]->m_VertexList[tempInt]->m_Position.y;

							m_MeshList[m_MeshList.size() - 1]->m_VertexList[m_MeshList[m_MeshList.size() - 1]->m_VertexList.size() - 1]->m_Normal.x = tempVec3.x;
							m_MeshList[m_MeshList.size() - 1]->m_VertexList[m_MeshList[m_MeshList.size() - 1]->m_VertexList.size() - 1]->m_Normal.z = tempVec3.y;
							m_MeshList[m_MeshList.size() - 1]->m_VertexList[m_MeshList[m_MeshList.size() - 1]->m_VertexList.size() - 1]->m_Normal.y = tempVec3.z;

							m_MeshList[m_MeshList.size() - 1]->m_VertexList[m_MeshList[m_MeshList.size() - 1]->m_VertexList.size() - 1]->m_Texture = m_MeshList[m_MeshList.size() - 1]->m_VertexList[tempInt]->m_Texture;

							m_MeshList[m_MeshList.size() - 1]->m_VertexList[m_MeshList[m_MeshList.size() - 1]->m_VertexList.size() - 1]->isVertexSet = true;

							// 기존 오리지널 버텍스에 현재 스플릿된 버텍스들 인덱스 리스트를 가지고 있기 위함
							m_MeshList[m_MeshList.size() - 1]->m_VertexList[tempInt]->m_splitList.push_back((unsigned int)m_MeshList[m_MeshList.size() - 1]->m_VertexList.size() - 1);
						}

						// 스플릿된 버텍스가 존재할 경우
						else if (m_MeshList[m_MeshList.size() - 1]->m_VertexList[tempInt]->m_splitList.size() > 1)
						{
							// 스플릿 리스트를 돌면서 순회
							for (int i = 0; i < m_MeshList[m_MeshList.size() - 1]->m_VertexList[tempInt]->m_splitList.size(); i++)
							{
								// 스플릿 리스트에서 현재 읽어들인 노말 값과 같은 버텍스가 있는 경우
								if (m_MeshList[m_MeshList.size() - 1]->m_VertexList[m_MeshList[m_MeshList.size() - 1]->m_VertexList[tempInt]->m_splitList[i]]->m_Normal.x == tempVec3.x &&
									m_MeshList[m_MeshList.size() - 1]->m_VertexList[m_MeshList[m_MeshList.size() - 1]->m_VertexList[tempInt]->m_splitList[i]]->m_Normal.y == tempVec3.z &&
									m_MeshList[m_MeshList.size() - 1]->m_VertexList[m_MeshList[m_MeshList.size() - 1]->m_VertexList[tempInt]->m_splitList[i]]->m_Normal.z == tempVec3.y)
								{
									tempBool = true;
									break;
								}
							}
							// 스플릿 리스트에서 현재 읽어들인 노말 값과 같은 버텍스가 없는 경우 (완전히 중복된 버텍스가 없는 경우)에만 버텍스 스플릿
							if (tempBool == false)
							{
								// 새로운 노말을 담을 버텍스를 추가해야 함
								CreateOneVertexToList();

								// 새로 만든 버텍스에 모든 정보를 옮겨 담음
								m_MeshList[m_MeshList.size() - 1]->m_VertexList[m_MeshList[m_MeshList.size() - 1]->m_VertexList.size() - 1]->m_Position.x = m_MeshList[m_MeshList.size() - 1]->m_VertexList[tempInt]->m_Position.x;
								m_MeshList[m_MeshList.size() - 1]->m_VertexList[m_MeshList[m_MeshList.size() - 1]->m_VertexList.size() - 1]->m_Position.z = m_MeshList[m_MeshList.size() - 1]->m_VertexList[tempInt]->m_Position.z;
								m_MeshList[m_MeshList.size() - 1]->m_VertexList[m_MeshList[m_MeshList.size() - 1]->m_VertexList.size() - 1]->m_Position.y = m_MeshList[m_MeshList.size() - 1]->m_VertexList[tempInt]->m_Position.y;

								m_MeshList[m_MeshList.size() - 1]->m_VertexList[m_MeshList[m_MeshList.size() - 1]->m_VertexList.size() - 1]->m_Normal.x = tempVec3.x;
								m_MeshList[m_MeshList.size() - 1]->m_VertexList[m_MeshList[m_MeshList.size() - 1]->m_VertexList.size() - 1]->m_Normal.z = tempVec3.y;
								m_MeshList[m_MeshList.size() - 1]->m_VertexList[m_MeshList[m_MeshList.size() - 1]->m_VertexList.size() - 1]->m_Normal.y = tempVec3.z;

								m_MeshList[m_MeshList.size() - 1]->m_VertexList[m_MeshList[m_MeshList.size() - 1]->m_VertexList.size() - 1]->m_Texture = m_MeshList[m_MeshList.size() - 1]->m_VertexList[tempInt]->m_Texture;

								m_MeshList[m_MeshList.size() - 1]->m_VertexList[m_MeshList[m_MeshList.size() - 1]->m_VertexList.size() - 1]->isVertexSet = true;

								// 기존 오리지널 버텍스에 현재 스플릿된 버텍스들 인덱스 리스트를 가지고 있기 위함
								m_MeshList[m_MeshList.size() - 1]->m_VertexList[tempInt]->m_splitList.push_back((unsigned int)m_MeshList[m_MeshList.size() - 1]->m_VertexList.size() - 1);
							}
						}
					}
				}

				// 페이스를 이루는 버텍스를 순회
				for (int i = 0; i < 3; i++)
				{
					// 페이스 리스트의 인덱스가 현재 읽어들인 *MESH_VERTEXNORMAL 인덱스와 일치할 경우
					if (m_MeshList[m_MeshList.size() - 1]->m_FaceList[faceIndex]->m_indexList[i] == tempInt)
					{
						// 그 페이스 인덱스의 버텍스와 같은 포지션을 가진 버텍스들을 순회
						for (int j = 1; j < m_MeshList[m_MeshList.size() - 1]->m_VertexList[tempInt]->m_splitList.size(); j++)
						{
							// *MESH_VERTEXNORMAL 노말 값과 일치하는 버텍스가 존재한다면
							if (m_MeshList[m_MeshList.size() - 1]->m_VertexList[m_MeshList[m_MeshList.size() - 1]->m_VertexList[tempInt]->m_splitList[j]]->m_Normal.x == tempVec3.x &&
								m_MeshList[m_MeshList.size() - 1]->m_VertexList[m_MeshList[m_MeshList.size() - 1]->m_VertexList[tempInt]->m_splitList[j]]->m_Normal.y == tempVec3.z &&
								m_MeshList[m_MeshList.size() - 1]->m_VertexList[m_MeshList[m_MeshList.size() - 1]->m_VertexList[tempInt]->m_splitList[j]]->m_Normal.z == tempVec3.y)
							{
								// 그 버텍스로 페이스 리스트의 인덱스를 교체
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

				// 여기서 버텍스 스플릿되어 추가된 버텍스에 weight 값들을 복사해서 넣어줌
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
	/// 버텍스 노말값과 페이스 노말값으로 비교했었는데 스키닝 실린더부터 버텍스 노말값과 페이스 노말값이 아예 다른 경우가 생겨
	/// 이렇게 하면 안되기에 TOKENR_MESH_VERTEXNORMAL 단계에서 이를 대체

	// 페이스를 순회하면서 
	for (int m = 0; m < m_MeshList.size(); m++)
	{
		//for (int i = 0; i < m_MeshList[m]->m_FaceList.size(); i++)
		//{
		//	for (int j = 0; j < 3; j++)
		//	{
		//		// 페이스를 이루는 각 버텍스 인덱스
		//		faceIndex = m_MeshList[m]->m_FaceList[i]->m_indexList[j];
		//		// 페이스를 이루는 그 버텍스가 가진 스플릿 사이즈
		//		splitSize = (unsigned int)m_MeshList[m]->m_VertexList[faceIndex]->m_splitList.size();
		//		for (int k = 0; k < splitSize; k++)
		//		{
		//			// 스플릿된 버텍스들을 순회하며
		//			tempInt3 = (unsigned int)m_MeshList[m]->m_VertexList[faceIndex]->m_splitList[k];
		//			// 페이스의 노말값과 같은 버텍스로 페이스를 이루는 버텍스의 인덱스를 교체해줌
		//			if (m_MeshList[m]->m_FaceList[i]->m_Normal == m_MeshList[m]->m_VertexList[tempInt3]->m_Normal)
		//			{
		//				m_MeshList[m]->m_FaceList[i]->m_indexList[j] = tempInt3;
		//				break;
		//			}
		//		}
		//	}
		//}

		// 스플릿 사이즈는 이제 필요없으므로 할당된 메모리를 지워주자
		for (int i = 0; i < m_MeshList[m]->m_VertexList.size(); i++)
		{
			// 빈 백터 생성
			std::vector<unsigned int> swapVector;
			// 빈 벡터로 스왑하여 기존 메모리 해제
			m_MeshList[m]->m_VertexList[i]->m_splitList.swap(swapVector);
		}
	}
}

void ASEParser::ApplyTextureNormal()
{
	// 텍스쳐가 존재할 경우에만 작동
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
					// 페이스가 가리키는 버텍스 인덱스
					faceIndex = m_MeshList[i]->m_FaceList[j]->m_indexList[k];
					// T페이스가 가리키는 버텍스 인덱스
					tFaceIndex = m_MeshList[i]->m_TfaceList[j]->m_indexList[k];

					// 페이스가 가리키는 버텍스 인덱스의 텍스쳐 = T페이스가 가리키는 버텍스 인덱스의 텍스쳐
					// 텍스쳐 값이 비어있다면 텍스쳐 값을 넣음
					if (m_MeshList[i]->m_VertexList[faceIndex]->m_Texture.x == 0 && m_MeshList[i]->m_VertexList[faceIndex]->m_Texture.y == 0)
					{
						m_MeshList[i]->m_VertexList[faceIndex]->m_Texture = m_MeshList[i]->m_TvertList[tFaceIndex]->m_UVW;

					}

					// 텍스쳐 값이 이미 차있다면, 즉 pos, norm이 같고 tex만 다른 버텍스가 존재한다면
					if (m_MeshList[i]->m_VertexList[faceIndex]->m_Texture.x != m_MeshList[i]->m_TvertList[tFaceIndex]->m_UVW.x || m_MeshList[i]->m_VertexList[faceIndex]->m_Texture.y != m_MeshList[i]->m_TvertList[tFaceIndex]->m_UVW.y)
					{
						if (tempBool == false)
						{
							// 텍스쳐쪽에서 버텍스 스플릿 하기 전 원본 버텍스 리스트의 크기를 저장
							startInt = (int)(m_MeshList[i]->m_VertexList.size() - 1);
							tempBool = true;
						}

						ASE_Parser::Vertex tempVertex;
						tempVertex.m_Position = m_MeshList[i]->m_VertexList[faceIndex]->m_Position;
						tempVertex.m_Normal = m_MeshList[i]->m_VertexList[faceIndex]->m_Normal;
						tempVertex.m_BoneWeightList = m_MeshList[i]->m_VertexList[faceIndex]->m_BoneWeightList;
						tempVertex.m_Texture = m_MeshList[i]->m_TvertList[tFaceIndex]->m_UVW;

						// 텍스쳐쪽에서 버텍스 스플릿을 한 후 생겨난 버텍스부터 버텍스 리스트를 순회
						for (int m = startInt; m < m_MeshList[i]->m_VertexList.size(); m++)
						{
							// 텍스쳐쪽에서 새로 생겨난 버텍스와 중복된 버텍스가 있는지 체크 
							if (m_MeshList[i]->m_VertexList[m]->m_Position == tempVertex.m_Position &&
								m_MeshList[i]->m_VertexList[m]->m_Normal == tempVertex.m_Normal &&
								m_MeshList[i]->m_VertexList[m]->m_Texture == tempVertex.m_Texture)
							{
								findRedundancy = true;
								// 중복된 버텍스가 있다면 그 중복된 버텍스로 페이스 인덱스를 교체해줌
								m_MeshList[i]->m_FaceList[j]->m_indexList[k] = m;
								break;
							}
						}

						// 중복된 버텍스가 없다면
						if (findRedundancy == false)
						{
							// 버텍스 추가
							CreateOneVertexToList(i);

							// pos, normal 값을 복사해옴
							m_MeshList[i]->m_VertexList[m_MeshList[i]->m_VertexList.size() - 1]->m_Position = tempVertex.m_Position;
							m_MeshList[i]->m_VertexList[m_MeshList[i]->m_VertexList.size() - 1]->m_Normal = tempVertex.m_Normal;
							m_MeshList[i]->m_VertexList[m_MeshList[i]->m_VertexList.size() - 1]->m_BoneWeightList = tempVertex.m_BoneWeightList;

							// texture 값은 새로운 값으로 채움
							m_MeshList[i]->m_VertexList[m_MeshList[i]->m_VertexList.size() - 1]->m_Texture = tempVertex.m_Texture;

							// 페이스에 들어있던 버텍스 인덱스를 현재 새로 생성한 버텍스로 교체해줌
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
		/// GEOMOBJECT의 부모는 GEOM이나 HELPER
		case eObjectState::GEOMOBJECT:
			// GEOM (MESH)에 부모가 존재한다면
			for (int i = 0; i < m_MeshList.size(); i++)
			{
				// 서치한 노드의 이름이 부모의 노드 이름과 같다면
				if (m_MeshList[i]->m_NodeName == parentName)
				{
					// 부모의 리스트에 추가
					baseObject->_parentObject.push_back((ASE_Parser::ASEOBJECT*)m_MeshList[i]);
					m_MeshList[i]->_childObject.push_back(baseObject);
					// 부모의 부모를 계속 타고 가면서 부모의 리스트에 추가시킴
					SetRelationShip(baseObject, (ASE_Parser::ASEOBJECT*)m_MeshList[i]);
					break;
				}
			}

			// Helper에 부모가 존재한다면
			for (int i = 0; i < m_HelperList.size(); i++)
			{
				// 서치한 노드의 이름이 부모의 노드 이름과 같다면
				if (m_HelperList[i]->m_NodeName == parentName)
				{
					// 부모의 리스트에 추가
					baseObject->_parentObject.push_back((ASE_Parser::ASEOBJECT*)m_HelperList[i]);
					m_HelperList[i]->_childObject.push_back(baseObject);
					// 부모의 부모를 계속 타고 가면서 부모의 리스트에 추가시킴
					SetRelationShip(baseObject, (ASE_Parser::ASEOBJECT*)m_HelperList[i]);
					break;
				}
			}
			break;

			/// HELPER의 부모는 HELPER나 SHAPE
		case eObjectState::HELPEROBEJCT:
			// Helper에 부모가 존재한다면
			for (int i = 0; i < m_HelperList.size(); i++)
			{
				// 서치한 노드의 이름이 부모의 노드 이름과 같다면
				if (m_HelperList[i]->m_NodeName == parentName)
				{
					// 부모의 리스트에 추가
					baseObject->_parentObject.push_back((ASE_Parser::ASEOBJECT*)m_HelperList[i]);
					m_HelperList[i]->_childObject.push_back(baseObject);
					// 부모의 부모를 계속 타고 가면서 부모의 리스트에 추가시킴
					SetRelationShip(baseObject, (ASE_Parser::ASEOBJECT*)m_HelperList[i]);
					break;
				}
			}

			// Shape에 부모가 존재한다면
			for (int i = 0; i < m_ShapeList.size(); i++)
			{
				// 서치한 노드의 이름이 부모의 노드 이름과 같다면
				if (m_ShapeList[i]->m_NodeName == parentName)
				{
					// 부모의 리스트에 추가
					baseObject->_parentObject.push_back((ASE_Parser::ASEOBJECT*)m_ShapeList[i]);
					m_ShapeList[i]->_childObject.push_back(baseObject);
					// 부모의 부모를 계속 타고 가면서 부모의 리스트에 추가시킴
					SetRelationShip(baseObject, (ASE_Parser::ASEOBJECT*)m_ShapeList[i]);
					break;
				}
			}
			break;

			/// SHAPE의 부모는 SHAPE, GEOMOBJECT(?)
		case eObjectState::SHAPEOBJECT:
			// Shape에 부모가 존재한다면
			for (int i = 0; i < m_ShapeList.size(); i++)
			{
				// 서치한 노드의 이름이 부모의 노드 이름과 같다면
				if (m_ShapeList[i]->m_NodeName == parentName)
				{
					// 부모의 리스트에 추가
					baseObject->_parentObject.push_back((ASE_Parser::ASEOBJECT*)m_ShapeList[i]);
					m_ShapeList[i]->_childObject.push_back(baseObject);
					// 부모의 부모를 계속 타고 가면서 부모의 리스트에 추가시킴
					SetRelationShip(baseObject, (ASE_Parser::ASEOBJECT*)m_ShapeList[i]);
					break;
				}
			}

			// GEOM에 부모가 존재한다면
			for (int i = 0; i < m_MeshList.size(); i++)
			{
				// 서치한 노드의 이름이 부모의 노드 이름과 같다면
				if (m_MeshList[i]->m_NodeName == parentName)
				{
					// 부모의 리스트에 추가
					baseObject->_parentObject.push_back((ASE_Parser::ASEOBJECT*)m_MeshList[i]);
					m_MeshList[i]->_childObject.push_back(baseObject);
					// 부모의 부모를 계속 타고 가면서 부모의 리스트에 추가시킴
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
	// row2와 row3을 바꾸며 y와 z의 위치를 바꿔야 함
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

		// 부모가 없는 오브젝트이면서 Bone이 아닐 경우
		if (m_MeshList[i]->_parentObject.empty())
		{
			// 그 오브젝트의 LocalTM은 WolrdTM과 같음
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

		// 부모가 없는 오브젝트라면
		if (m_HelperList[i]->_parentObject.empty())
		{
			// 그 오브젝트의 LocalTM은 WolrdTM과 같음
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

		// 부모가 없는 오브젝트라면
		if (m_ShapeList[i]->_parentObject.empty())
		{
			// 그 오브젝트의 LocalTM은 WolrdTM과 같음
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
			// 부모가 존재하는 오브젝트라면 부모를 순회하며 부모의 역행렬을 가장 먼 부모에서부터 곱해줌
			for (int j = (int)m_MeshList[i]->_parentObject.size() - 1; j >= 0; j--)
			{
				// 부모의 Local행렬을 가져옴
				DirectX::XMMATRIX parentLocal = DirectX::XMLoadFloat4x4(&m_MeshList[i]->_parentObject[j]->_originLocalTM);
				// 부모의 Local 행렬의 역행렬
				DirectX::XMMATRIX parentLocalInverse = DirectX::XMMatrixInverse(nullptr, parentLocal);
				// 내 World에서 부모의 Local의 역행렬을 곱해줌
				myWorld = DirectX::XMMatrixMultiply(myWorld, parentLocalInverse);
			}
			// 결과 값을 내 LocalTM에 저장
			DirectX::XMStoreFloat4x4(&m_MeshList[i]->_originLocalTM, myWorld);
		}
	}

	for (int i = 0; i < m_HelperList.size(); i++)
	{
		if (m_HelperList[i]->_parentObject.empty() == false)
		{
			DirectX::XMMATRIX myWorld = DirectX::XMLoadFloat4x4(&m_HelperList[i]->_originWorldTM);
			// 부모가 존재하는 오브젝트라면 부모를 순회하며 부모의 역행렬을 가장 먼 부모에서부터 곱해줌
			for (int j = (int)m_HelperList[i]->_parentObject.size() - 1; j >= 0; j--)
			{
				// 부모의 Local행렬을 가져옴
				DirectX::XMMATRIX parentLocal = DirectX::XMLoadFloat4x4(&m_HelperList[i]->_parentObject[j]->_originLocalTM);
				// 부모의 Local 행렬의 역행렬
				DirectX::XMMATRIX parentLocalInverse = DirectX::XMMatrixInverse(nullptr, parentLocal);
				// 내 World에서 부모의 Local의 역행렬을 곱해줌
				myWorld = DirectX::XMMatrixMultiply(myWorld, parentLocalInverse);
			}
			// 결과 값을 내 LocalTM에 저장
			DirectX::XMStoreFloat4x4(&m_HelperList[i]->_originLocalTM, myWorld);
		}
	}

	for (int i = 0; i < m_ShapeList.size(); i++)
	{
		if (m_ShapeList[i]->_parentObject.empty() == false)
		{
			DirectX::XMMATRIX myWorld = DirectX::XMLoadFloat4x4(&m_ShapeList[i]->_originWorldTM);
			// 부모가 존재하는 오브젝트라면 부모를 순회하며 부모의 역행렬을 가장 먼 부모에서부터 곱해줌
			for (int j = (int)m_ShapeList[i]->_parentObject.size() - 1; j >= 0; j--)
			{
				// 부모의 Local행렬을 가져옴
				DirectX::XMMATRIX parentLocal = DirectX::XMLoadFloat4x4(&m_ShapeList[i]->_parentObject[j]->_originLocalTM);
				// 부모의 Local 행렬의 역행렬
				DirectX::XMMATRIX parentLocalInverse = DirectX::XMMatrixInverse(nullptr, parentLocal);
				// 내 World에서 부모의 Local의 역행렬을 곱해줌
				myWorld = DirectX::XMMatrixMultiply(myWorld, parentLocalInverse);
			}
			// 결과 값을 내 LocalTM에 저장
			DirectX::XMStoreFloat4x4(&m_ShapeList[i]->_originLocalTM, myWorld);
		}
	}
}

void ASEParser::SetNewWorldTM()
{
	for (int i = 0; i < m_MeshList.size(); i++)
	{
		// 내 로컬부터 부모의 로컬을 곱해주기위해 초기 월드 값으로 내 로컬을 들고 있음
		m_MeshList[i]->_newWorldTM.at(_meshCount) = m_MeshList[i]->_originLocalTM;

		if (m_MeshList[i]->_parentObject.empty() == false)
		{
			for (int j = 0; j < m_MeshList[i]->_parentObject.size(); j++)

			{
				// 내 Local과 부모의 Local 행렬을 가져옴
				DirectX::XMMATRIX myWorld = DirectX::XMLoadFloat4x4(&m_MeshList[i]->_newWorldTM.at(_meshCount));
				DirectX::XMMATRIX parentLocal = DirectX::XMLoadFloat4x4(&m_MeshList[i]->_parentObject[j]->_originLocalTM);
				// 내 Local과 부모의 Local을 곱한 행렬을 NewWorld에 저장
				DirectX::XMStoreFloat4x4(&m_MeshList[i]->_newWorldTM.at(_meshCount), DirectX::XMMatrixMultiply(myWorld, parentLocal));
			}
		}
	}

	for (int i = 0; i < m_HelperList.size(); i++)
	{
		// 내 로컬부터 부모의 로컬을 곱해주기위해 초기 월드 값으로 내 로컬을 들고 있음
		m_HelperList[i]->_newWorldTM.at(_meshCount) = m_HelperList[i]->_originLocalTM;

		if (m_HelperList[i]->_parentObject.empty() == false)
		{
			for (int j = 0; j < m_HelperList[i]->_parentObject.size(); j++)
			{
				// 내 Local과 부모의 Local 행렬을 가져옴
				DirectX::XMMATRIX myWorld = DirectX::XMLoadFloat4x4(&m_HelperList[i]->_newWorldTM.at(_meshCount));
				DirectX::XMMATRIX parentLocal = DirectX::XMLoadFloat4x4(&m_HelperList[i]->_parentObject[j]->_originLocalTM);
				// 내 Local과 부모의 Local을 곱한 행렬을 NewWorld에 저장
				DirectX::XMStoreFloat4x4(&m_HelperList[i]->_newWorldTM.at(_meshCount), DirectX::XMMatrixMultiply(myWorld, parentLocal));
			}
		}
	}

	for (int i = 0; i < m_ShapeList.size(); i++)
	{
		// 내 로컬부터 부모의 로컬을 곱해주기위해 초기 월드 값으로 내 로컬을 들고 있음
		m_ShapeList[i]->_newWorldTM.at(_meshCount) = m_ShapeList[i]->_originLocalTM;

		if (m_ShapeList[i]->_parentObject.empty() == false)
		{
			for (int j = 0; j < m_ShapeList[i]->_parentObject.size(); j++)
			{
				// 내 Local과 부모의 Local 행렬을 가져옴
				DirectX::XMMATRIX myWorld = DirectX::XMLoadFloat4x4(&m_ShapeList[i]->_newWorldTM.at(_meshCount));
				DirectX::XMMATRIX parentLocal = DirectX::XMLoadFloat4x4(&m_ShapeList[i]->_parentObject[j]->_originLocalTM);
				// 내 Local과 부모의 Local을 곱한 행렬을 NewWorld에 저장
				DirectX::XMStoreFloat4x4(&m_ShapeList[i]->_newWorldTM.at(_meshCount), DirectX::XMMatrixMultiply(myWorld, parentLocal));
			}
		}
	}
}


void ASEParser::SetSpecificNewWorldTM(ASE_Parser::ASEOBJECT* pMesh, int meshIndex)
{
	// 로컬TM이 바뀐 상태이므로 로컬TM을 다시 NewWorldTM에 저장함
	DirectX::XMMATRIX myLocal = DirectX::XMLoadFloat4x4(&pMesh->_localTM.at(meshIndex));
	DirectX::XMStoreFloat4x4(&pMesh->_newWorldTM.at(meshIndex), myLocal);

	for (int i = 0; i < pMesh->_parentObject.size(); i++)
	{
		// 부모의 로컬TM을 담고
		DirectX::XMMATRIX myWorld = DirectX::XMLoadFloat4x4(&pMesh->_newWorldTM.at(meshIndex));
		DirectX::XMMATRIX parentLocal = DirectX::XMLoadFloat4x4(&pMesh->_parentObject[i]->_localTM.at(meshIndex));
		// 내 Local과 부모의 Local을 곱한 행렬을 NewWorld에 저장
		DirectX::XMStoreFloat4x4(&pMesh->_newWorldTM.at(meshIndex), DirectX::XMMatrixMultiply(myWorld, parentLocal));
	}

	while (1)
	{
		// 자식을 계속 타고가서 자식이 없다면 탈출
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
	// 로컬TM이 바뀐 상태이므로 로컬TM을 다시 NewWorldTM에 저장함
	DirectX::XMMATRIX mySlerp = DirectX::XMLoadFloat4x4(&pMesh->_slerpTM.at(meshIndex));
	DirectX::XMStoreFloat4x4(&pMesh->_newWorldTM.at(meshIndex), mySlerp);

	for (int i = 0; i < pMesh->_parentObject.size(); i++)
	{
		// 부모의 로컬TM을 담고
		DirectX::XMMATRIX myWorld = DirectX::XMLoadFloat4x4(&pMesh->_newWorldTM.at(meshIndex));
		DirectX::XMMATRIX parentLocal = DirectX::XMLoadFloat4x4(&pMesh->_parentObject[i]->_slerpTM.at(meshIndex));
		// 내 Local과 부모의 Local을 곱한 행렬을 NewWorld에 저장
		DirectX::XMStoreFloat4x4(&pMesh->_newWorldTM.at(meshIndex), DirectX::XMMatrixMultiply(myWorld, parentLocal));
	}

	while (1)
	{
		// 자식을 계속 타고가서 자식이 없다면 탈출
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
			// 월드의 역행렬 도출
			DirectX::XMMATRIX myWorld = XMLoadFloat4x4(&m_MeshList[i]->_originWorldTM);
			DirectX::XMMATRIX myWorldInverse = XMMatrixInverse(nullptr, myWorld);
			// 포지션 
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

			// 로컬인 포지션에 월드행렬을 곱해서 월드 위치로 보냄
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
							// Bone List에 명시된 본의 부모들은 본의 역할을 하기에 렌더링 할 필요가 없기에 본의 부모들도 본으로 판단
							// 다이나믹 캐스트로 하려고 했는데 virtual 상속관계가 아니라 불가능하여 이렇게 함
							ASE_Parser::Mesh* parentObject = (ASE_Parser::Mesh*)(m_MeshList[k]->_parentObject[m]);
							parentObject->m_isBone = true;
						}

						// 자식은 어떻게 해야하나? 부모가 본이면 자식도 무조건 본?
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

