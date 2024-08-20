/// <summary>
/// 리소스 관리를 위한 매니저
/// 렌더링에 필요한 리소스들을 지니고 있을 예정
/// 
/// 윤종화
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

	// 비교 연산자 구현
	bool operator==(const TextPosition& other) const {
		return x == other.x && y == other.y;
	}
};

// Point 구조체를 사용하는 해시 함수 구현
namespace std
{
	template <>
	struct hash<TextPosition>
	{
		std::size_t operator()(const TextPosition& p) const
		{
			std::size_t h1 = std::hash<int>()(p.x);
			std::size_t h2 = std::hash<int>()(p.y);
			// 해시 함수를 조합하여 최종 해시 값을 반환
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
		// IResource 형으로 저장되어있는 리소스를 원형으로 반환해줌
		template <typename Resource>
		Resource* GetResource(const std::string& fileName);

		// IResource 형으로 리소스를 백터에 저장함
		void SetResource(const std::string& fileName, IResource* resource);

		// 폴더에서 특정 확장자의 파일을 모두 찾아 그 이름들을 백터로 넘겨줌
		std::vector<std::string> FindSpecificResources(const std::string& folderName, const std::string& extension);

		// 백터에 있는 특정 리소스를 삭제
		void DeleteResource(const std::string& fileName);

	public:
		// 렌더링 할 오브젝트들은 담아놓은 백터 반환
		const std::vector<ArkEngine::IRenderable*>& GetRenderableList();

		// 렌더링 할 오브젝트들을 담아놓은 백터
		void AddRenderable(ArkEngine::IRenderable* renderable);

		// 특정 렌더링하던 오브젝트를 삭제
		void DeleteRenderable(ArkEngine::IRenderable* renderable);

		unsigned int GetObjectIndex();

	public:
		// 인스턴싱을 활용하여 렌더링하기 위한 renderer를 가져옴
		std::vector<ArkEngine::MeshRenderer*>& GetAllMeshRenderer();
		// 인스턴싱을 활용하여 렌더링하기 위한 renderer를 가져옴
		ArkEngine::MeshRenderer* GetMeshRenderer(const std::string& fileName);
		// 인스턴싱을 활용하여 렌더링하기 위한 renderer를 만듬
		void AddMeshRenderer(ArkEngine::MeshRenderer* meshRenderer);
		void SortMeshRendererByAlpha();

		std::vector<ArkEngine::MeshRenderer*>& GetAllTransParentRenderer();
		std::vector<ArkEngine::MeshRenderer*>& GetAllNoTransParentRenderer();

	public:
		// 모든 디버그 오브젝트들은 담아놓은 백터 반환
		std::vector<ArkEngine::IDebugObject*>& GetDebugObjectList();

		void AddDebugObject(ArkEngine::IDebugObject* object);

		void DeleteDebugObject(ArkEngine::IDebugObject* object);

	public:
		// 모든 라인 오브젝트들은 담아놓은 백터 반환
		const std::vector<ArkEngine::ILineObject*>& GetLineObjectList();

		void AddLineObject(ArkEngine::ILineObject* object);

		void DeleteLineObject(ArkEngine::ILineObject* object);

	public:
		// 모든 디버그 오브젝트들은 담아놓은 백터 반환
		const std::vector<ArkEngine::IUIImage*>& GetUIImageList();

		void AddUIImage(ArkEngine::IUIImage* image);

		void DeleteUIImage(ArkEngine::IUIImage* image);

	public:
		// 모든 큐브맵을 담아놓은 백터 반환
		const std::unordered_set<ArkEngine::ICubeMap*>& GetCubeMapList();

		const std::vector<std::string>& GetCubeMapNameList();

		// 현재 렌더링되고 있는 큐브맵을 반환
		ArkEngine::ICubeMap* GetNowCubeMap();

		void SetNowCubeMap(ArkEngine::ICubeMap* cubeMap);

		void SetCubeMapNameList(std::vector <std::string> nameVector);

		// 큐브맵 백터에 큐브맵을 추가
		void AddCubeMap(ArkEngine::ICubeMap* cubeMap);

		// 큐브맵 백터에서 큐브맵을 삭제
		void DeleteCubeMap(ArkEngine::ICubeMap* cubeMap);

	public:
		// 렌더링 할 오브젝트들은 담아놓은 백터 반환
		const std::vector<ArkEngine::ICamera*>& GetCameraList();

		// 렌더링 할 오브젝트들을 담아놓은 백터
		void AddCamera(ArkEngine::ICamera* camera);

		std::vector<ArkEngine::ICamera*>& GetShadowCamera();

		// 그림자 용 카메라를 추가 현재는 한개만 가능
		void SetShadowCamera(ArkEngine::ICamera* camera);

		// 특정 렌더링하던 오브젝트를 삭제
		void DeleteCamera(ArkEngine::ICamera* camera);

	public:
		const std::unordered_map<TextPosition, std::string>& GetTextList();
		// 텍스트 리스트에 텍스트 추가
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
		// 에디터에 모델 이름을 넘겨주기 위함
		std::vector<std::string> GetRenderableNameList();

		void SetTextureNameList(std::string name);

		// 에디터에 텍스쳐 이름을 넘겨주기 위함
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
		// 렌더링 할 오브젝트들은 담아놓은 백터 반환
		const std::vector<ArkEngine::ParticleSystem*>& GetParticleList();

		// 렌더링 할 오브젝트들을 담아놓은 백터
		void AddParticle(ArkEngine::ParticleSystem* particle);

		// 특정 렌더링하던 오브젝트를 삭제
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
		// 존재하는 모든 그릴 수 있는 오브젝트들
		std::vector<IRenderable*> _renderableList;

		std::vector<MeshRenderer*> _meshRendererList;

		std::vector<MeshRenderer*> _transParentList;
		std::vector<MeshRenderer*> _noTransParentList;

		std::vector<IDebugObject*> _debugList;

		std::vector<ILineObject*> _lineList;

		std::vector<IUIImage*> _uiImageList;

		// 존재하는 모든 큐브맵
		ICubeMap* _nowCubeMap;

		std::unordered_set<ICubeMap*> _cubeMapList;

		std::vector<std::string> _cubeMapNameList;

		// 존재하는 모든 카메라 오브젝트들
		std::vector<ICamera*> _cameraList;

		std::vector<ArkEngine::ICamera*> _shadowCamera;

		// 출력할 텍스트 리스트
		std::unordered_map<TextPosition, std::string> _textList;

		// ASE 파서 리스트
		std::unordered_map<std::string, ASEParser*> _ASEParserList;

		// Fbx에서 파싱된 모델들에 대한 리스트
		std::unordered_map < std::string, std::vector<ModelMesh*>> _fbxParsingList;

		// FBX에서 읽어온 애니메이션 정보들
		std::unordered_map<std::wstring, AnimData*> _animDataList;

		// FBX에서 읽어온 메테리얼 정보들
		std::unordered_map<std::wstring, ModelMaterial*> _materialList;

		// FBX용 아크버퍼 백터를 담아놓기
		std::unordered_map <std::string, std::vector<ArkEngine::ArkDX11::ArkBuffer*>> _arkBufferList;

	private:
		// 존재하는 모든 텍스쳐 이름 리스트 // 갖고있는게 비효율적인가 고민 중
		std::vector<std::string> _textureNameList;

	private:
		// 모든 파티클 리스트
		std::vector<ArkEngine::ParticleSystem*> _particleList;
		// Particle에서 사용되는 리소스 리스트
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

