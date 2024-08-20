#pragma once
#include <windows.h>
#include <vector>
#include <directxmath.h>

#include "NavigationStruct.h"
#include "CommonHeader.h"

/// 이것들은 샘플 간에 일관된 값 사용을 위한 샘플 영역입니다.
/// 사용자는 자신의 요구에 따라 이를 지정해야 합니다.
///
/// 샘플로 제공된 예시 enum타입을 그대로 들고왔습니다.
/// 아직 이 enum 변수들이 무슨 역할을 하는지 모르는 상황이므로, 일단 옮겨서 사용되는 모습을 보고 분석할것입니다.
/// 

namespace KunrealEngine
{
	struct _DECLSPEC Agent
	{
		float _agentHeight = 2.0f;
		float _agentRadius = 1.2f;
		float _agentMaxClimb = 0.9f;
		float _agentMaxSlope = 45.0f;
	};

	// 이 PathFIndbox는 네비매쉬를 배열로 관리하기 편하도록 구성 요소들을 묶은 것
	struct _DECLSPEC PathFindPack
	{
		class dtNavMesh* _navMesh;
		class dtNavMeshQuery* _navQuery;
		
		// 에이전트 세팅
		Agent _agentsetting;

		// PathFind를 위해 필요한 부분들
		static const int MAX_POLYS = 256;
		static const int MAX_SMOOTH = 2048;

		dtPolyRef _startRef;
		dtPolyRef _endRef;
		dtPolyRef _parent[MAX_POLYS];
		float _startPos[3];
		float _endPos[3];
		dtQueryFilter _filter;
		dtPolyRef _path[MAX_POLYS];
		int _pathCount;
		float _polyPickExt[3];

		// Straight-pathfind를 위해 필요한 부분들
		float _straightPath[MAX_POLYS * 3];
		unsigned char _straightPathFlags[MAX_POLYS];
		dtPolyRef _straightPathPolys[MAX_POLYS];
		int _nstraightPath;

		// Raycast-pathfind를 위해 필요한 부분들
		dtRaycastHit _hit;
		dtPolyRef _RaycastPathPolys;
		float _hitPos[3];

		// Temp-Obstacle을 위해 필요한 부분
		class dtTileCache* _tileCache;

		PathFindPack();
		~PathFindPack();
	};

	class _DECLSPEC Navigation
	{
	private:
		Navigation();
		~Navigation();

	public:
		// 싱글턴 객체 반환 함수
		static Navigation& GetInstance();
		// 초기화 함수.
		void Initialize();
		// 제거 함수
		void Release();

		// path 에서 이미 빌드된 네비매쉬 파일을 읽어옵니다. .bin파일
		void LoadAll(const char* path, int index);
		// path 에서 빌드된 네비매쉬 파일을 저장합니다. .bin파일
		void SaveAll(int index, const char* path);
		// 네비매쉬를 직접 빌드 하기
		bool HandleBuild(int index, std::string path);
		// 네비매쉬를 업데이트 한다.
		void HandleUpdate(const float dt);

		// 장애물을 추가한다.
		// pos = 장애물 위치 / radius = 장애물 크기 / height = 장애물 사이즈
		void AddTempObstacle(DirectX::XMFLOAT3 pos, float radius, float height);
		// pos = 장애물 위치 / bmin = 장애물 최소좌표 / bmax = 장애물 최대좌표
		void AddBoxTempObstacle(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 bmin, DirectX::XMFLOAT3 bmax);
		// 특정 장애물을 제거한다.
		void RemoveTempObstacle(DirectX::XMFLOAT3 pos);
		// bpos에 있는 장애물의 위치를 npos의 위치로 옮긴다.
		void MoveTempObstacle(DirectX::XMFLOAT3 bpos, DirectX::XMFLOAT3 npos);
		// 모든 장애물을 제거한다.
		void ClearAllTempObstacles();

		// 직선경로 탐색 함수
		std::vector<std::pair<DirectX::XMFLOAT3, DirectX::XMFLOAT3>> FindStraightPath(int index);
		// 이미 탐색된 경로를 가져오는 함수
		std::vector<std::pair<DirectX::XMFLOAT3, DirectX::XMFLOAT3>> GetPath(int index);
		// Raycast 탐색 함수. (직선경로에 부딧히는게 있다면 거기까지만 경로 표시
		DirectX::XMFLOAT3 FindRaycastPath(int index);

		// startpos 와 endpos를 입력하는 함수. float[3] 버전
		void SetSEpos(int index, float sx, float sy, float sz, float ex, float ey, float ez);
		// startpos 와 endpos를 입력하는 함수. XMFLOAT3 버전
		void SetSEpos(int index, DirectX::XMFLOAT3 startPosition, DirectX::XMFLOAT3 endPosition);
		// startpos를 입력하는 함수. float[3] 버전
		void SetStartpos(int index, float x, float y, float z);
		// startpos를 입력하는 함수. XMFLOAT3 버전
		void SetStartpos(int index, DirectX::XMFLOAT3 position);
		// endpos를 입력하는 함수. float[3] 버전
		void SetEndpos(int index, float x, float y, float z);
		// endpos를 입력하는 함수. XMFLOAT3 버전
		void SetEndpos(int index, DirectX::XMFLOAT3 position);
		// 네비매쉬를 빌드하기 위한 agent를 세팅하는 함수. 각 변수명을 참고
		void SetAgent(int index, float agentHeight, float agentMaxSlope, float agentRadius, float agentMaxClimb);
		
		// 좌표값을 입력해서 그 값이 네비매쉬 위에 있는지 판단하는 함수들 
		unsigned int GetPositionRef(int index, float x, float y, float z);	// Ref값을 반환
		bool GetPositionOnMap(int index, float x, float y, float z);// 단순이 네비매쉬 위에 있는지 아닌지만 반환

		// 네비매쉬를 빌드하기 위해 vertex와 index를 제공해주는 함수(예정)
		void GetNavmeshRenderInfo(int index, std::vector<DirectX::XMFLOAT3>& vertices, std::vector<unsigned int>& indices);
		// 패키지(네비매쉬 배열) 사이즈 가져오기
		int GetPackageSize();
		// 현재 에이전트 세팅을 반환
		void GetAgent(int index, float& agentHeight, float& agentMaxSlope, float& agentRadius, float& agentMaxClimb);
		// 현재 네비매쉬 폴더 내 저장되어 있는 네비매쉬들을 가져옴
		std::vector<std::string> GetNavimeshPathList();

		std::vector<std::string> GetMapObjPathList();
	
	private:	// 이거는 private라고!!!!! 가져다 쓰다가 소문자로 시작하는 함수 보이면 쓰지 말라고!!!

		int rasterizeTileLayers(const int tx, const int ty, const rcConfig& cfg, struct TileCacheData* tiles, const int maxTiles);
		void getTilePos(const float* pos, int& tx, int& ty);
		void renderCachedTile(const int tx, const int ty, const int type);
		void renderCachedTileOverlay(const int tx, const int ty, double* proj, double* model, int* view);
		// sp, sq를 기반으로 장애물의 ref 값을 반환하는 함수. sp,sq가 뭔진 아직 나도 모르겠음
		dtObstacleRef hitTestObstacle(const dtTileCache* tc, const float* sq);
		static bool isectSegAABB(const float* sq,const float* amin, const float* amax,float& tmin, float& tmax);
		
		bool compareXMFLOAT3(DirectX::XMFLOAT3 v1, DirectX::XMFLOAT3 v2);
		// float*를 XMFLOAT3로 변환 해주는 짤막 함수 
		DirectX::XMFLOAT3 vertex(const float* pos);
	
	private:
		static const int PACKAGESIZE = 5;

		class InputGeom* _geom;
		PathFindPack _package[PACKAGESIZE];

		bool _keepInterResults;
		float _totalBuildTimeMs;

		unsigned char* _triareas;
		rcHeightfield* _solid;
		rcCompactHeightfield* _chf;
		rcContourSet* _cset;
		rcPolyMesh* _pmesh;
		rcConfig _cfg;
		rcPolyMeshDetail* _dmesh;
		rcContext* _ctx;

		unsigned char _navMeshDrawFlags;

		float _cellSize;
		float _cellHeight;
		float _regionMinSize;
		float _regionMergeSize;
		float _edgeMaxLen;
		float _edgeMaxError;
		float _vertsPerPoly;
		float _detailSampleDist;
		float _detailSampleMaxError;
		int _partitionType;

		bool _filterLowHangingObstacles;
		bool _filterLedgeSpans;
		bool _filterWalkableLowHeightSpans;

		// solo_mesh -> temp+obstacle로 바꾸면서 추가된 부분
		struct LinearAllocator* _talloc;
		struct FastLZCompressor* _tcomp;
		struct MeshProcess* _tmproc;

		float _cacheBuildTimeMs;
		int _cacheCompressedSize;
		int _cacheRawSize;
		int _cacheLayerCount;
		unsigned int _cacheBuildMemUsage;

		int _maxTiles;
		int _maxPolysPerTile;
		float _tileSize = 48;
	};
}