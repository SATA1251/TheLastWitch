/// <summary>
/// 게임 엔진과 그래픽스 엔진을 연결해주는 인터페이스
/// 상의하에 계속해서 추가하고 수정해 나갈 예정
/// 
/// 이건호 윤종화
/// </summary>

/// <summary>
/// IRenderable = 3D 오브젝트
/// </summary>

#pragma once
#include <string>
#include <vector>

namespace DirectX
{
	struct XMFLOAT3;
	struct XMFLOAT4;
	struct XMMATRIX;
}

namespace ArkEngine
{
	class IRenderable;
}

namespace GInterface
{
	class GraphicsRenderable;
	class GraphicsCamera;
	class GraphicsDirLight;
	class GraphicsPointLight;
	class GraphicsDebug;
	class GraphicsImage;
	class GraphicsParticle;
	class GraphicsTransparentMesh;

	class GraphicsInterface
	{
	public:
		virtual ~GraphicsInterface() {}

	public:
		// 그래픽스 엔진 생성 후 초기화 할 때 사용
		virtual void Initialize(long long hwnd, int clientWidth, int clientHeight) abstract;
		// 그래픽스 엔진 생성 후 원하는 색깔로 초기화 할 때 사용
		virtual void Initialize(long long hwnd, int clientWidth, int clientHeight, float backGroundColor[4]) abstract;
		// 그래픽스 엔진 내의 모든 객체 업데이트
		virtual void Update() abstract;
		// 그래픽스 엔진 내의 모든 객체 렌더링
		virtual void Render() abstract;
		// 그래픽스 엔진이 종료될 때 사용
		virtual void Finalize() abstract;

	public:
		// 렌더러블 오브젝트를 생성 후 이를 반환
		virtual GInterface::GraphicsRenderable* CreateRenderable(const char* fileName, bool isSold = true) abstract;
		// 렌더러블 오브젝트 삭제
		virtual void DeleteRenderable(GInterface::GraphicsRenderable* renderable) abstract;
		// 피킹된 렌더러블 오브젝트 반환
		virtual GInterface::GraphicsRenderable* GetPickedRenderable(int mouseX, int mouseY) abstract;

	public:
		// 디버그 큐브 생성 후 반환 ** 꼭 실제 오브젝트 모양대로 이름 만들기!**
		virtual GInterface::GraphicsDebug* CreateDebugCube(const char* objectName, float width, float height, float depth) abstract;
		virtual GInterface::GraphicsDebug* CreateDebugSphere(const char* objectName, float radius) abstract;
		// 버텍스와 인덱스를 받아 디버그 오브젝트를 생성 후 이를 반환
		virtual GInterface::GraphicsDebug* CreateMapDebug(const char* objectName, std::vector<DirectX::XMFLOAT3>& vertexVec, std::vector<unsigned int>& indexVec) abstract;

		virtual void DeleteDebugObject(GInterface::GraphicsDebug* debugObject) abstract;

		virtual GInterface::GraphicsDebug* GetDebugObject(const std::string& name) abstract;

		virtual void DeleteDebugMap(const std::string& name) abstract;

		virtual void SetDeltaTime(float dt) abstract;

	public:
		// 두 버텍스로 원하는 컬러로 라인을 생성
		virtual void CreateDebugLine(const DirectX::XMFLOAT3& vertex1, const DirectX::XMFLOAT3& vertex2, const DirectX::XMFLOAT4& color) abstract;
		// 만들어놓은 순서대로 index가 부여, 그 index를 통해 라인을 삭제
		virtual void DeleteLine(int index) abstract;
		// 만들어놓은 라인과 일치하는 버텍스를 지닌 라인을 삭제
		virtual void DeleteLine(const DirectX::XMFLOAT3& vertex1, const DirectX::XMFLOAT3& vertex2) abstract;
		// 모든 라인을 제거
		virtual void DeleteAllLine() abstract;

	public:
		// UI Image 오브젝트 생성 후 반환
		virtual GInterface::GraphicsImage* CreateImage(const char* imageName) abstract;
		// UI Image 오브젝트 삭제
		virtual void DeleteImage(GInterface::GraphicsImage* image) abstract;
		// 피킹된 UI Image 오브젝트 반환
		virtual GInterface::GraphicsImage* GetPickedImage(int mouseX, int mouseY) abstract;

	public:
		// 큐브맵 생성
		virtual void CreateCubeMap(const char* cubeMapName, const char* textureName="", bool isCube = false) abstract;
		// 큐브맵 삭제
		virtual void DeleteCubeMap(const char* cubeMapName) abstract;
		// 큐브맵 리스트 가져오기
		virtual const std::vector<std::string>& GetCubeMapList() abstract;
		// 현재 렌더링 되는 메인 큐브맵 가져오기
		virtual void SetMainCubeMap(std::string cubeMapName) abstract;

	public:
		// 카메라를 생성 후 반환
		virtual GInterface::GraphicsCamera* CreateCamera(const DirectX::XMFLOAT3& cameraPosition = { 0.0f, 0.0f, 0.0f }, const DirectX::XMFLOAT3& targetPosition = { 0.0f, 0.0f, 0.0f }) abstract;
		// 카메라 객체를 삭제
		virtual void DeleteCamera(GInterface::GraphicsCamera* camera) abstract;
		// 현재 렌더링하고 있는 메인 카메라 가져오기
		virtual GInterface::GraphicsCamera* GetMainCamera() abstract;
		// 매개인자로 넣은 카메라를 메인 카메라로 설정
		virtual void SetMainCamera(GInterface::GraphicsCamera* camera) abstract;

	public:
		// Directional Light 생성
		virtual GInterface::GraphicsDirLight* CreateDirectionalLight(const DirectX::XMFLOAT4& ambient, const DirectX::XMFLOAT4& diffuse, const DirectX::XMFLOAT4& specular, const DirectX::XMFLOAT3& direction) abstract;
		virtual GInterface::GraphicsPointLight* CreatePointLight(const DirectX::XMFLOAT4& ambient, const DirectX::XMFLOAT4& diffuse, const DirectX::XMFLOAT4& specular, const DirectX::XMFLOAT3& position, float range, float attenuation) abstract;

	public:
		// 파티클 생성
		virtual GInterface::GraphicsParticle* CreateParticle(const std::string& particleName, const std::string& fileName, unsigned int maxParticle) abstract;
		// 파티클 삭제
		virtual void DeleteParticle(GInterface::GraphicsParticle* particle) abstract;

	public:
		// End 키를 통해 텍스트 출력 활성화 / 비활성화
		virtual void DrawDebugText(int posX, int posY, int fontSize, const char* text, ...) abstract;
		// 색깔이 입혀진 텍스트 출력 (RGBA 0~255)
		virtual void DrawUIText(int posX, int posY, int fontSize, const DirectX::XMFLOAT4& color, const char* text, ...) abstract;

	public:
		// IMGUI에서 렌더링시키기 위해 이미지를 넘겨줌
		virtual void* GetRenderingImage() abstract;

		// IMGUI에 디바이스를 넘겨주기 위함
		virtual void* GetDevice() abstract;

		// IMGUI에 디바이스 컨텍스트를 넘겨주기 위함
		virtual void* GetDeviceContext() abstract;

		// IMGUI에 RenderTargetView를 넘겨주기 위함
		virtual void* GetRenderTargetView() abstract;

		// IMGUI에 그릴 수 있는 파싱된 오브젝트의 파일명을 넘겨주기 위함
		virtual const std::vector<std::string> GetRenderableNameList() abstract;

		// IMGUI에 그릴 수 있는 파싱된 텍스쳐의 파일명을 넘겨주기 위함
		virtual const std::vector<std::string>& GetTextureNameList() abstract;

		// screen 마우스 좌표를 world 좌표로 변환
		virtual const DirectX::XMFLOAT3 ScreenToWorldPoint(int mouseX, int mouseY) abstract;

		virtual const DirectX::XMFLOAT3 ScreenToWorldPoint(int mouseX, int mouseY, float planeD) abstract;

		// IMGUIZMO를 사용하기 위해 메인 카메라의 View행렬을 반환
		virtual const DirectX::XMFLOAT4X4& GetViewMatrix() abstract;

		// IMGUIZMO를 사용하기 위해 메인 카메라의 Proj행렬을 반환
		virtual const DirectX::XMFLOAT4X4& GetProjMatrix() abstract;


		// 특정 본을 따라가도록 하는 Transform 행렬 반환
		virtual const DirectX::XMFLOAT4X4& GetTransform(GInterface::GraphicsRenderable* renderable, const std::string& boneName) abstract;

		// 네비 메쉬를 위한 모든 메쉬들의 버텍스의 월드좌표 반환
		virtual const std::vector<std::vector<std::vector<DirectX::XMFLOAT3>>> GetAllMeshVertex() abstract;
		// 네비 메쉬를 위한 모든 인덱스들의 버텍스의 월드좌표 반환
		virtual const std::vector<std::vector<std::vector<unsigned int>>> GetAllMeshIndex() abstract;

		virtual std::vector<DirectX::XMFLOAT3> GetMeshVertexData(const std::string& fileName) abstract;
		
		virtual std::vector<unsigned int> GetMeshIndexData(const std::string& fileName) abstract;
		
		// 투명 오브젝트 생성 후 반환	// 오브젝트 이름, 텍스쳐 이름, max 투명도, 원형인가 여부
		virtual GInterface::GraphicsTransparentMesh* CreateTransParentMesh(const std::string& objectName, const std::string& textureName, float transParency = 1.0f, bool isCircle = false) abstract;
		// 투명 오브젝트 삭제
		virtual void DeleteTransParentMesh(GInterface::GraphicsTransparentMesh* mesh) abstract;
	};
};

	// 그래픽스 엔진을 생성한 후 반환
	extern "C" __declspec(dllexport) GInterface::GraphicsInterface* CreateGraphicsEngine();

	// 그래픽스 엔진을 종료
	extern "C" __declspec(dllexport) void ReleaseGraphicsEngine(GInterface::GraphicsInterface* instance);