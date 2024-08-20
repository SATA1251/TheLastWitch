#include "ParsingDataStruct.h"

ASE_Parser::Vertex::Vertex()
	: m_Position(), m_Normal(), m_Texture(), isVertexSet(false), m_splitList()
{

}


ASE_Parser::Vertex::~Vertex()
{
	std::vector<unsigned int> unsInt;
	m_splitList.swap(unsInt);
}

ASE_Parser::Face::Face()
	: m_indexList(), m_Normal()
{
	std::vector<unsigned int> unsInt;
	m_indexList.swap(unsInt);
}

ASE_Parser::Tvert::Tvert()
	: m_UVW()
{

}

ASE_Parser::Tface::Tface()
	: m_indexList()
{
	std::vector<unsigned int> unsInt;
	m_indexList.swap(unsInt);
}

ASE_Parser::ASEOBJECT::ASEOBJECT()
	: m_NodeName(), m_NodeParent(), m_Tm_Row0(), m_Tm_Row1(), m_Tm_Row2(), m_Tm_Row3(), m_Tm_Pos(), m_Tm_RotAxis(), m_Tm_RotAngle(0.0f), m_Tm_Scale(), m_Tm_ScaleAxis(), m_Tm_ScaleAxisAng(0.0f), _originWorldTM(), _localTM(), _newWorldTM(), _originLocalTM(),
	_slerpTM(), m_nextFrame_POS_Index(0), m_nextFrame_Rot_Index(0)
{
}

ASE_Parser::ASEOBJECT::~ASEOBJECT()
{
	std::vector<ASEOBJECT*> aseObject;
	_parentObject.swap(aseObject);
}

ASE_Parser::Mesh::Mesh()
	: ASEOBJECT(), m_TotalVertexNum(0), m_TotalFaceNum(0), m_TotalBoneNum(0), m_isBone(false), m_TempBoneIndex(0)
{

}

ASE_Parser::Mesh::~Mesh()
{
	std::vector<Vertex*> ver;
	m_VertexList.swap(ver);
	std::vector<Face*> face;
	m_FaceList.swap(face);
	std::vector<Tvert*> tVert;
	m_TvertList.swap(tVert);
	std::vector<Tface*> tFace;
	m_TfaceList.swap(tFace);
}

ASE_Parser::Helper::Helper()
	: ASEOBJECT()
{

}

ASE_Parser::Shape::Shape()
	: ASEOBJECT()
{

}

ASE_Parser::BoneWeight::BoneWeight(unsigned int index, float weight)
	: m_BoneIndex(index), m_Weight(weight)
{

}

ASE_Parser::Material::Material()
	: m_name(), m_ambient(0.0f, 0.0f, 0.0f, 1.0f), m_diffuse(0.0f, 0.0f, 0.0f, 1.0f), m_specular(0.0f, 0.0f, 0.0f, 1.0f)
{

}
