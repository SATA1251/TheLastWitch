/// <summary>
/// ���ҽ� ������ ���� �Ŵ���
/// �������� �ʿ��� ���ҽ����� ���ϰ� ���� ����
/// 
/// ����ȭ
/// </summary>

#pragma once
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <vector>
#include "DXTKFont.h"
#include "IUIImage.h"
#include "IResource.h"
#include "IRenderable.h"
#include "IDebugObject.h"
#include "ICamera.h"
#include "ICubeMap.h"
#include "ILineObject.h"

class ASEParser;

namespace ArkEngine
{
	class MeshRenderer;
	class ParticleResource;
	class ParticleSystem;

	namespace ArkDX11
	{
		class ArkBuffer;
		class TransparentMesh;
	}
}

struct ModelMesh;

struct ModelMaterial;

struct AnimData;

struct TextPosition {
	int x;
	int y;

	// �� ������ ����
	bool operator==(const TextPosition& other) const {
		return x == other.x && y == other.y;
	}
};

// Point ����ü�� ����ϴ� �ؽ� �Լ� ����
namespace std
{
	template <>
	struct hash<TextPosition>
	{
		std::size_t operator()(const TextPosition& p) const
		{
			std::size_t h1 = std::hash<int>()(p.x);
			std::size_t h2 = std::hash<int>()(p.y);
			// �ؽ� �Լ��� �����Ͽ� ���� �ؽ� ���� ��ȯ
			return h1 ^ (h2 << 1);
		}
	};
}


namespace ArkEngine
{
	class ResourceManager
	{
	private:
		ResourceManager();
		~ResourceManager();

		static ResourceManager* instance;

	public:
		static ResourceManager* GetInstance();
		static void Delete();

	public:
		// IResource ������ ����Ǿ��ִ� ���ҽ��� �������� ��ȯ����
		template <typename Resource>
		Resource* GetResource(const std::string& fileName);

		// IResource ������ ���ҽ��� ���Ϳ� ������
		void SetResource(const std::string& fileName, IResource* resource);

		// �������� Ư�� Ȯ������ ������ ��� ã�� �� �̸����� ���ͷ� �Ѱ���
		std::vector<std::string> FindSpecificResources(const std::string& folderName, const std::string& extension);

		// ���Ϳ� �ִ� Ư�� ���ҽ��� ����
		void DeleteResource(const std::string& fileName);

	public:
		// ������ �� ������Ʈ���� ��Ƴ��� ���� ��ȯ
		const std::vector<ArkEngine::IRenderable*>& GetRenderableList();

		// ������ �� ������Ʈ���� ��Ƴ��� ����
		void AddRenderable(ArkEngine::IRenderable* renderable);

		// Ư�� �������ϴ� ������Ʈ�� ����
		void DeleteRenderable(ArkEngine::IRenderable* renderable);

		unsigned int GetObjectIndex();

	public:
		// �ν��Ͻ��� Ȱ���Ͽ� �������ϱ� ���� renderer�� ������
		std::vector<ArkEngine::MeshRenderer*>& GetAllMeshRenderer();
		// �ν��Ͻ��� Ȱ���Ͽ� �������ϱ� ���� renderer�� ������
		ArkEngine::MeshRenderer* GetMeshRenderer(const std::string& fileName);
		// �ν��Ͻ��� Ȱ���Ͽ� �������ϱ� ���� renderer�� ����
		void AddMeshRenderer(ArkEngine::MeshRenderer* meshRenderer);
		void SortMeshRendererByAlpha();

		std::vector<ArkEngine::MeshRenderer*>& GetAllTransParentRenderer();
		std::vector<ArkEngine::MeshRenderer*>& GetAllNoTransParentRenderer();

	public:
		// ��� ����� ������Ʈ���� ��Ƴ��� ���� ��ȯ
		std::vector<ArkEngine::IDebugObject*>& GetDebugObjectList();

		void AddDebugObject(ArkEngine::IDebugObject* object);

		void DeleteDebugObject(ArkEngine::IDebugObject* object);

	public:
		// ��� ���� ������Ʈ���� ��Ƴ��� ���� ��ȯ
		const std::vector<ArkEngine::ILineObject*>& GetLineObjectList();

		void AddLineObject(ArkEngine::ILineObject* object);

		void DeleteLineObject(ArkEngine::ILineObject* object);

	public:
		// ��� ����� ������Ʈ���� ��Ƴ��� ���� ��ȯ
		const std::vector<ArkEngine::IUIImage*>& GetUIImageList();

		void AddUIImage(ArkEngine::IUIImage* image);

		void DeleteUIImage(ArkEngine::IUIImage* image);

	public:
		// ��� ť����� ��Ƴ��� ���� ��ȯ
		const std::unordered_set<ArkEngine::ICubeMap*>& GetCubeMapList();

		const std::vector<std::string>& GetCubeMapNameList();

		// ���� �������ǰ� �ִ� ť����� ��ȯ
		ArkEngine::ICubeMap* GetNowCubeMap();

		void SetNowCubeMap(ArkEngine::ICubeMap* cubeMap);

		void SetCubeMapNameList(std::vector <std::string> nameVector);

		// ť��� ���Ϳ� ť����� �߰�
		void AddCubeMap(ArkEngine::ICubeMap* cubeMap);

		// ť��� ���Ϳ��� ť����� ����
		void DeleteCubeMap(ArkEngine::ICubeMap* cubeMap);

	public:
		// ������ �� ������Ʈ���� ��Ƴ��� ���� ��ȯ
		const std::vector<ArkEngine::ICamera*>& GetCameraList();

		// ������ �� ������Ʈ���� ��Ƴ��� ����
		void AddCamera(ArkEngine::ICamera* camera);

		std::vector<ArkEngine::ICamera*>& GetShadowCamera();

		// �׸��� �� ī�޶� �߰� ����� �Ѱ��� ����
		void SetShadowCamera(ArkEngine::ICamera* camera);

		// Ư�� �������ϴ� ������Ʈ�� ����
		void DeleteCamera(ArkEngine::ICamera* camera);

	public:
		const std::unordered_map<TextPosition, std::string>& GetTextList();
		// �ؽ�Ʈ ����Ʈ�� �ؽ�Ʈ �߰�
		void SetTextList(int posX, int posY, std::string text);

	public:
		ASEParser* GetASEParser(const std::string& fileName);

		void AddASEParser(const std::string& fileName, ASEParser* parser);

		void DeleteASEParser(const std::string& fileName);

	public:
		const std::vector<ModelMesh*>& GetFbxParsingData(const std::string& fileName);

		void AddFbxParsingData(const std::string& fileName, std::vector<ModelMesh*> modelData);

		void DeleteFbxParsingData(const std::string& fileName);

	public:
		// �����Ϳ� �� �̸��� �Ѱ��ֱ� ����
		std::vector<std::string> GetRenderableNameList();

		void SetTextureNameList(std::string name);

		// �����Ϳ� �ؽ��� �̸��� �Ѱ��ֱ� ����
		const std::vector<std::string>& GetTextureNameList();

	public:
		AnimData* GetAnimationData(const std::wstring& fileName);

		void AddAnimationData(const std::wstring& fileName, AnimData* data);

		void DeleteAnimationData(const std::wstring& fileName);

	public:
		ModelMaterial* GetModelMaterial(const std::wstring& fileName);

		void AddModelMaterial(const std::wstring& fileName, ModelMaterial* data);

		void DeleteModelMaterial(const std::wstring& fileName);

	public:
		const std::vector<ArkEngine::ArkDX11::ArkBuffer*>& GetArkBuffer(const std::string& bufferName);
		void AddArkBuffer(const std::string& bufferName, ArkEngine::ArkDX11::ArkBuffer* buffer);

	public:
		// ������ �� ������Ʈ���� ��Ƴ��� ���� ��ȯ
		const std::vector<ArkEngine::ParticleSystem*>& GetParticleList();

		// ������ �� ������Ʈ���� ��Ƴ��� ����
		void AddParticle(ArkEngine::ParticleSystem* particle);

		// Ư�� �������ϴ� ������Ʈ�� ����
		void DeleteParticle(ArkEngine::ParticleSystem* particle);

	public:
		ParticleResource* GetParticleResource(const std::string& particleName);
		void AddParticleResource(const std::string& particleName, ParticleResource* particleResource);

	public:
		const std::vector<ArkEngine::ArkDX11::TransparentMesh*>& GetTransParentMeshList();
		void AddTransParentMesh(ArkEngine::ArkDX11::TransparentMesh* mesh);
		void DeleteTransParentMesh(ArkEngine::ArkDX11::TransparentMesh* mesh);

		const std::vector<ArkEngine::ArkDX11::TransparentMesh*>& GetDecalMeshList();
		void AddDecalMesh(ArkEngine::ArkDX11::TransparentMesh* mesh);
		void ClearDecalMeshVec();

		void SortTransParentMesh();

		void SortDecalMesh();

	public:
		void ReleaseAll();

	private:
		std::unordered_map<std::string, IResource*> _resourceList;

	private:
		// �����ϴ� ��� �׸� �� �ִ� ������Ʈ��
		std::vector<IRenderable*> _renderableList;

		std::vector<MeshRenderer*> _meshRendererList;

		std::vector<MeshRenderer*> _transParentList;
		std::vector<MeshRenderer*> _noTransParentList;

		std::vector<IDebugObject*> _debugList;

		std::vector<ILineObject*> _lineList;

		std::vector<IUIImage*> _uiImageList;

		// �����ϴ� ��� ť���
		ICubeMap* _nowCubeMap;

		std::unordered_set<ICubeMap*> _cubeMapList;

		std::vector<std::string> _cubeMapNameList;

		// �����ϴ� ��� ī�޶� ������Ʈ��
		std::vector<ICamera*> _cameraList;

		std::vector<ArkEngine::ICamera*> _shadowCamera;

		// ����� �ؽ�Ʈ ����Ʈ
		std::unordered_map<TextPosition, std::string> _textList;

		// ASE �ļ� ����Ʈ
		std::unordered_map<std::string, ASEParser*> _ASEParserList;

		// Fbx���� �Ľ̵� �𵨵鿡 ���� ����Ʈ
		std::unordered_map < std::string, std::vector<ModelMesh*>> _fbxParsingList;

		// FBX���� �о�� �ִϸ��̼� ������
		std::unordered_map<std::wstring, AnimData*> _animDataList;

		// FBX���� �о�� ���׸��� ������
		std::unordered_map<std::wstring, ModelMaterial*> _materialList;

		// FBX�� ��ũ���� ���͸� ��Ƴ���
		std::unordered_map <std::string, std::vector<ArkEngine::ArkDX11::ArkBuffer*>> _arkBufferList;

	private:
		// �����ϴ� ��� �ؽ��� �̸� ����Ʈ // �����ִ°� ��ȿ�����ΰ� ��� ��
		std::vector<std::string> _textureNameList;

	private:
		// ��� ��ƼŬ ����Ʈ
		std::vector<ArkEngine::ParticleSystem*> _particleList;
		// Particle���� ���Ǵ� ���ҽ� ����Ʈ
		std::unordered_map<std::string, ParticleResource*> _particleResourceList;

		std::vector<ArkEngine::ArkDX11::TransparentMesh*> _transParentMeshList;

		std::vector<ArkEngine::ArkDX11::TransparentMesh*> _decalMeshList;

	private:
		unsigned int _objectIndex;
	};

	template <typename Resource>
	Resource* ArkEngine::ResourceManager::GetResource(const std::string& fileName)
	{
		auto iter = _resourceList.find(fileName);

		if (iter != _resourceList.end())
		{
			return dynamic_cast<Resource*>(_resourceList.at(fileName));
		}
		else
		{
			return nullptr;
		}
	}
}

