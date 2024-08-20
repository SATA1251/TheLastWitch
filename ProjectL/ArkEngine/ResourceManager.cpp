#include <filesystem>
#include <algorithm>
#include "ASEParser.h"
#include "ArkBuffer.h"
#include "ParsingStructs.h"
#include "MeshRenderer.h"
#include "TransparentMesh.h"
#include "ResourceManager.h"

ArkEngine::ResourceManager* ArkEngine::ResourceManager::instance = nullptr;

ArkEngine::ResourceManager::ResourceManager()
	: _nowCubeMap(nullptr), _objectIndex(0)
{

}

ArkEngine::ResourceManager::~ResourceManager()
{

}


ArkEngine::ResourceManager* ArkEngine::ResourceManager::GetInstance()
{
	if (instance == nullptr)
	{
		instance = new ArkEngine::ResourceManager();
	}

	return instance;
}


void ArkEngine::ResourceManager::Delete()
{
	delete instance;
}

void ArkEngine::ResourceManager::SetResource(const std::string& fileName, IResource* resource)
{
	_resourceList.insert(std::make_pair(fileName, resource));
}

std::vector<std::string> ArkEngine::ResourceManager::FindSpecificResources(const std::string& folderName, const std::string& extension)
{
	std::string folderPath = folderName + "/";
	std::string targetExtension = "." + extension;

	std::vector<std::string> filenameList;

	for (const auto& entry : std::filesystem::directory_iterator(folderPath))
	{
		if (entry.is_regular_file() && entry.path().extension() == targetExtension)
		{
			filenameList.emplace_back(folderPath + entry.path().filename().string());
		}
		else if (entry.is_directory())
		{
			std::vector<std::string> subfolderFiles = FindSpecificResources(entry.path().string(), extension);

			filenameList.insert(filenameList.end(), subfolderFiles.begin(), subfolderFiles.end());
		}
	}

	return filenameList;
}

void ArkEngine::ResourceManager::DeleteResource(const std::string& fileName)
{
	auto iter = _resourceList.find(fileName);

	if (iter != _resourceList.end())
	{
		delete iter->second;
		_resourceList.erase(iter);
	}
}

const std::vector<ArkEngine::IRenderable*>& ArkEngine::ResourceManager::GetRenderableList()
{
	return _renderableList;
}

void ArkEngine::ResourceManager::AddRenderable(ArkEngine::IRenderable* renderable)
{
	_renderableList.emplace_back(renderable);
	_objectIndex++;
}

void ArkEngine::ResourceManager::DeleteRenderable(ArkEngine::IRenderable* renderable)
{
	_renderableList.erase(std::remove(_renderableList.begin(), _renderableList.end(), renderable), _renderableList.end());

	delete renderable;
}

unsigned int ArkEngine::ResourceManager::GetObjectIndex()
{
	return _objectIndex;
}


std::vector<ArkEngine::MeshRenderer*>& ArkEngine::ResourceManager::GetAllMeshRenderer()
{
	return _meshRendererList;
}


ArkEngine::MeshRenderer* ArkEngine::ResourceManager::GetMeshRenderer(const std::string& fileName)
{
	for (auto& index : _meshRendererList)
	{
		if (index->GetName() == fileName)
		{
			return index;
		}
	}

	return nullptr;
}

void ArkEngine::ResourceManager::AddMeshRenderer(ArkEngine::MeshRenderer* meshRenderer)
{
	_meshRendererList.emplace_back(meshRenderer);
}


void ArkEngine::ResourceManager::SortMeshRendererByAlpha()
{
	_transParentList.clear();
	_noTransParentList.clear();

	for (auto& index : _meshRendererList)
	{
		if (!index->GetAlphaExist())
		{
			_noTransParentList.emplace_back(index);
		}
		else
		{
			_transParentList.emplace_back(index);
		}
	}

	std::sort(_transParentList.begin(), _transParentList.end(),
		[](MeshRenderer* mesh1, MeshRenderer* mesh2) {
			return mesh1->GetDepth() > mesh2->GetDepth();
		});

	_meshRendererList.clear();

	_meshRendererList.reserve(_noTransParentList.size() + _transParentList.size());

	_meshRendererList.insert(_meshRendererList.end(), _noTransParentList.begin(), _noTransParentList.end());

	_meshRendererList.insert(_meshRendererList.end(), _transParentList.begin(), _transParentList.end());
}


std::vector<ArkEngine::MeshRenderer*>& ArkEngine::ResourceManager::GetAllTransParentRenderer()
{
	return _transParentList;
}


std::vector<ArkEngine::MeshRenderer*>& ArkEngine::ResourceManager::GetAllNoTransParentRenderer()
{
	return _noTransParentList;
}

std::vector<ArkEngine::IDebugObject*>& ArkEngine::ResourceManager::GetDebugObjectList()
{
	return _debugList;
}

void ArkEngine::ResourceManager::AddDebugObject(ArkEngine::IDebugObject* object)
{
	_debugList.emplace_back(object);
}

void ArkEngine::ResourceManager::DeleteDebugObject(ArkEngine::IDebugObject* object)
{
	_debugList.erase(std::remove(_debugList.begin(), _debugList.end(), object), _debugList.end());
}

const std::vector<ArkEngine::ILineObject*>& ArkEngine::ResourceManager::GetLineObjectList()
{
	return _lineList;
}

void ArkEngine::ResourceManager::AddLineObject(ArkEngine::ILineObject* object)
{
	_lineList.emplace_back(object);
}

void ArkEngine::ResourceManager::DeleteLineObject(ArkEngine::ILineObject* object)
{
	auto it = std::find(_lineList.begin(), _lineList.end(), object);
	if (it != _lineList.end()) 
	{
		delete* it;
		_lineList.erase(it);
	}
}

const std::vector<ArkEngine::IUIImage*>& ArkEngine::ResourceManager::GetUIImageList()
{
	return _uiImageList;
}

void ArkEngine::ResourceManager::AddUIImage(ArkEngine::IUIImage* image)
{
	_uiImageList.emplace_back(image);
	_objectIndex++;
}

void ArkEngine::ResourceManager::DeleteUIImage(ArkEngine::IUIImage* image)
{
	auto it = std::find(_uiImageList.begin(), _uiImageList.end(), image);
	if (it != _uiImageList.end())
	{
		_uiImageList.erase(it);
	}
}

const std::unordered_set<ArkEngine::ICubeMap*>& ArkEngine::ResourceManager::GetCubeMapList()
{
	return _cubeMapList;
}

const std::vector<std::string>& ArkEngine::ResourceManager::GetCubeMapNameList()
{
	return _cubeMapNameList;
}

ArkEngine::ICubeMap* ArkEngine::ResourceManager::GetNowCubeMap()
{
	return _nowCubeMap;
}

void ArkEngine::ResourceManager::SetNowCubeMap(ArkEngine::ICubeMap* cubeMap)
{
	_nowCubeMap = cubeMap;
}

void ArkEngine::ResourceManager::SetCubeMapNameList(std::vector <std::string> nameVector)
{
	_cubeMapNameList = nameVector;
}

void ArkEngine::ResourceManager::AddCubeMap(ArkEngine::ICubeMap* cubeMap)
{
	for (const auto& index : _cubeMapList)
	{
		if (index->GetName() == cubeMap->GetName())
		{
			DeleteCubeMap(index);
			break;
		}
	}

	_cubeMapList.insert(cubeMap);
	_cubeMapNameList.emplace_back(cubeMap->GetName());
}

void ArkEngine::ResourceManager::DeleteCubeMap(ArkEngine::ICubeMap* cubeMap)
{
	_cubeMapNameList.erase(std::remove(_cubeMapNameList.begin(), _cubeMapNameList.end(), cubeMap->GetName()), _cubeMapNameList.end());
	_cubeMapList.erase(cubeMap);
	
	delete cubeMap;
}

const std::vector<ArkEngine::ICamera*>& ArkEngine::ResourceManager::GetCameraList()
{
	return _cameraList;
}

void ArkEngine::ResourceManager::AddCamera(ArkEngine::ICamera* camera)
{
	_cameraList.emplace_back(camera);
}

std::vector<ArkEngine::ICamera*>& ArkEngine::ResourceManager::GetShadowCamera()
{
	return _shadowCamera;
}

void ArkEngine::ResourceManager::SetShadowCamera(ArkEngine::ICamera* camera)
{
	_shadowCamera.emplace_back(camera);
}

void ArkEngine::ResourceManager::DeleteCamera(ArkEngine::ICamera* camera)
{
	if (camera->GetMain() == false)
	{
		auto iter = find(_cameraList.begin(), _cameraList.end(), camera);
		if (iter != _cameraList.end())
		{
			_cameraList.erase(std::remove(_cameraList.begin(), _cameraList.end(), camera), _cameraList.end());
			delete camera;

			return;
		}		
	}

	auto iter = find(_shadowCamera.begin(), _shadowCamera.end(), camera);
	if (iter != _shadowCamera.end())
	{
		_shadowCamera.erase(std::remove(_shadowCamera.begin(), _shadowCamera.end(), camera), _shadowCamera.end());
		delete camera;

		return;
	}

}

const std::unordered_map<TextPosition, std::string>& ArkEngine::ResourceManager::GetTextList()
{
	return _textList;
}

void ArkEngine::ResourceManager::SetTextList(int posX, int posY, std::string text)
{
	TextPosition temp;
	temp.x = posX;
	temp.y = posY;

	auto it = _textList.find(temp);

	if (it != _textList.end())
	{
		_textList.at(temp) = text;
	}
	else
	{
		_textList.insert(std::make_pair(temp, text));
	}
}

ASEParser* ArkEngine::ResourceManager::GetASEParser(const std::string& fileName)
{
	auto iter = _ASEParserList.find(fileName);

	if (iter != _ASEParserList.end())
	{
		return iter->second;
	}
	
	else return nullptr;
}

void ArkEngine::ResourceManager::AddASEParser(const std::string& fileName, ASEParser* parser)
{
	_ASEParserList.insert(std::make_pair(fileName, parser));
}

void ArkEngine::ResourceManager::DeleteASEParser(const std::string& fileName)
{
	auto iter = _ASEParserList.find(fileName);

	if (iter != _ASEParserList.end())
	{
		delete iter->second;
		_ASEParserList.erase(iter);
	}
}

const std::vector<ModelMesh*>& ArkEngine::ResourceManager::GetFbxParsingData(const std::string& fileName)
{
	auto iter = _fbxParsingList.find(fileName);
	
	if (iter != _fbxParsingList.end())
	{
		return iter->second;
	}
	
	else
	{
		std::vector<ModelMesh*> tempVec;
		return tempVec;
	}
}

void ArkEngine::ResourceManager::AddFbxParsingData(const std::string& fileName, std::vector<ModelMesh*> modelData)
{
	_fbxParsingList.insert(std::make_pair(fileName, modelData));
}

void ArkEngine::ResourceManager::DeleteFbxParsingData(const std::string& fileName)
{
	auto iter = _fbxParsingList.find(fileName);
	
	if (iter != _fbxParsingList.end())
	{
		for (const auto& index : iter->second)
		{
			iter->second.erase(std::remove(iter->second.begin(), iter->second.end(), index), iter->second.end());
			
			delete index;
		}
		_fbxParsingList.erase(iter);
	}
}

std::vector<std::string> ArkEngine::ResourceManager::GetRenderableNameList()
{
	std::vector<std::string> nameList;

	for (const auto& pair : _ASEParserList)
	{
		nameList.emplace_back(pair.first);
	}

	return nameList;
}

const std::vector<std::string>& ArkEngine::ResourceManager::GetTextureNameList()
{
	return _textureNameList;
}

AnimData* ArkEngine::ResourceManager::GetAnimationData(const std::wstring& fileName)
{
	auto iter = _animDataList.find(fileName);

	if (iter != _animDataList.end())
	{
		return iter->second;
	}

	else return nullptr;
}

void ArkEngine::ResourceManager::AddAnimationData(const std::wstring& fileName, AnimData* data)
{
	_animDataList.insert(std::make_pair(fileName, data));
}

void ArkEngine::ResourceManager::DeleteAnimationData(const std::wstring& fileName)
{
	auto iter = _animDataList.find(fileName);

	if (iter != _animDataList.end())
	{
		delete iter->second;
		_animDataList.erase(iter);
	}
}

ModelMaterial* ArkEngine::ResourceManager::GetModelMaterial(const std::wstring& fileName)
{
	auto iter = _materialList.find(fileName);

	if (iter != _materialList.end())
	{
		return iter->second;
	}

	else return nullptr;
}

void ArkEngine::ResourceManager::AddModelMaterial(const std::wstring& fileName, ModelMaterial* data)
{
	_materialList.insert(std::make_pair(fileName, data));
}

void ArkEngine::ResourceManager::DeleteModelMaterial(const std::wstring& fileName)
{
	auto iter = _materialList.find(fileName);

	if (iter != _materialList.end())
	{
		delete iter->second;
		_materialList.erase(iter);
	}
}

const std::vector<ArkEngine::ArkDX11::ArkBuffer*>& ArkEngine::ResourceManager::GetArkBuffer(const std::string& bufferName)
{
	return _arkBufferList.at(bufferName);
}

void ArkEngine::ResourceManager::AddArkBuffer(const std::string& bufferName, ArkEngine::ArkDX11::ArkBuffer* buffer)
{
	const auto& iter = _arkBufferList.find(bufferName);

	if (iter == _arkBufferList.end())
	{
		std::vector<ArkEngine::ArkDX11::ArkBuffer*> bufferList;
		_arkBufferList.insert(std::make_pair(bufferName, bufferList));
	}

	_arkBufferList.at(bufferName).emplace_back(buffer);
}


const std::vector<ArkEngine::ParticleSystem*>& ArkEngine::ResourceManager::GetParticleList()
{
	return _particleList;
}


void ArkEngine::ResourceManager::AddParticle(ArkEngine::ParticleSystem* particle)
{
	_particleList.emplace_back(particle);
}


void ArkEngine::ResourceManager::DeleteParticle(ArkEngine::ParticleSystem* particle)
{
	_particleList.erase(std::remove(_particleList.begin(), _particleList.end(), particle), _particleList.end());

	delete particle;
}

ArkEngine::ParticleResource* ArkEngine::ResourceManager::GetParticleResource(const std::string& particleName)
{
	auto iter = _particleResourceList.find(particleName);

	if (iter != _particleResourceList.end())
	{
		return iter->second;
	}

	else return nullptr;
}


void ArkEngine::ResourceManager::AddParticleResource(const std::string& particleName, ParticleResource* particleResource)
{
	const auto& iter = _particleResourceList.find(particleName);

	if (iter == _particleResourceList.end())
	{
		_particleResourceList.insert(std::make_pair(particleName, particleResource));
	}
}


const std::vector<ArkEngine::ArkDX11::TransparentMesh*>& ArkEngine::ResourceManager::GetTransParentMeshList()
{
	return _transParentMeshList;
}

void ArkEngine::ResourceManager::AddTransParentMesh(ArkEngine::ArkDX11::TransparentMesh* mesh)
{
	_transParentMeshList.emplace_back(mesh);
}

void ArkEngine::ResourceManager::DeleteTransParentMesh(ArkEngine::ArkDX11::TransparentMesh* mesh)
{
	_transParentMeshList.erase(std::remove(_transParentMeshList.begin(), _transParentMeshList.end(), mesh), _transParentMeshList.end());

	delete mesh;
}


const std::vector<ArkEngine::ArkDX11::TransparentMesh*>& ArkEngine::ResourceManager::GetDecalMeshList()
{
	return _decalMeshList;
}


void ArkEngine::ResourceManager::AddDecalMesh(ArkEngine::ArkDX11::TransparentMesh* mesh)
{
	auto it = std::find(_decalMeshList.begin(), _decalMeshList.end(), mesh);

	if (it == _decalMeshList.end()) 
	{
		_decalMeshList.emplace_back(mesh);
	}
}


void ArkEngine::ResourceManager::ClearDecalMeshVec()
{
	_decalMeshList.clear();
}

void ArkEngine::ResourceManager::SortTransParentMesh()
{
	std::sort(_transParentMeshList.begin(), _transParentMeshList.end(),
		[](ArkEngine::ArkDX11::TransparentMesh* mesh1, ArkEngine::ArkDX11::TransparentMesh* mesh2) {
			return mesh1->GetTransParency() > mesh2->GetTransParency();
		});
}


void ArkEngine::ResourceManager::SortDecalMesh()
{
	std::sort(_decalMeshList.begin(), _decalMeshList.end(),
		[](ArkEngine::ArkDX11::TransparentMesh* mesh1, ArkEngine::ArkDX11::TransparentMesh* mesh2) {
			return mesh1->GetIndex() < mesh2->GetIndex();
		});
}

void ArkEngine::ResourceManager::SetTextureNameList(std::string name)
{
	_textureNameList.emplace_back(name);
}

void ArkEngine::ResourceManager::ReleaseAll()
{
	_textList.clear();

	for (const auto& index : _renderableList)
	{
		delete index;
	}
	_renderableList.clear();

	_cubeMapNameList.clear();

	for (const auto& index : _cubeMapList)
	{
		delete index;
	}
	_cubeMapList.clear();

	for (const auto& index : _cameraList)
	{
		delete index;
	}
	_cameraList.clear();

	auto aseIt = _ASEParserList.begin();

	while (aseIt != _ASEParserList.end())
	{
		delete aseIt->second;

		aseIt->second = nullptr;
		aseIt = _ASEParserList.erase(aseIt);
	}

	auto it = _resourceList.begin();

	while (it != _resourceList.end())
	{
		it->second->UnLoad(it->first.c_str());
		it = _resourceList.begin();
	}
}