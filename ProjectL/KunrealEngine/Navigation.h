#pragma once
#include <windows.h>
#include <vector>
#include <directxmath.h>

#include "NavigationStruct.h"
#include "CommonHeader.h"

/// �̰͵��� ���� ���� �ϰ��� �� ����� ���� ���� �����Դϴ�.
/// ����ڴ� �ڽ��� �䱸�� ���� �̸� �����ؾ� �մϴ�.
///
/// ���÷� ������ ���� enumŸ���� �״�� ���Խ��ϴ�.
/// ���� �� enum �������� ���� ������ �ϴ��� �𸣴� ��Ȳ�̹Ƿ�, �ϴ� �Űܼ� ���Ǵ� ����� ���� �м��Ұ��Դϴ�.
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

	// �� PathFIndbox�� �׺�Ž��� �迭�� �����ϱ� ���ϵ��� ���� ��ҵ��� ���� ��
	struct _DECLSPEC PathFindPack
	{
		class dtNavMesh* _navMesh;
		class dtNavMeshQuery* _navQuery;
		
		// ������Ʈ ����
		Agent _agentsetting;

		// PathFind�� ���� �ʿ��� �κе�
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

		// Straight-pathfind�� ���� �ʿ��� �κе�
		float _straightPath[MAX_POLYS * 3];
		unsigned char _straightPathFlags[MAX_POLYS];
		dtPolyRef _straightPathPolys[MAX_POLYS];
		int _nstraightPath;

		// Raycast-pathfind�� ���� �ʿ��� �κе�
		dtRaycastHit _hit;
		dtPolyRef _RaycastPathPolys;
		float _hitPos[3];

		// Temp-Obstacle�� ���� �ʿ��� �κ�
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
		// �̱��� ��ü ��ȯ �Լ�
		static Navigation& GetInstance();
		// �ʱ�ȭ �Լ�.
		void Initialize();
		// ���� �Լ�
		void Release();

		// path ���� �̹� ����� �׺�Ž� ������ �о�ɴϴ�. .bin����
		void LoadAll(const char* path, int index);
		// path ���� ����� �׺�Ž� ������ �����մϴ�. .bin����
		void SaveAll(int index, const char* path);
		// �׺�Ž��� ���� ���� �ϱ�
		bool HandleBuild(int index, std::string path);
		// �׺�Ž��� ������Ʈ �Ѵ�.
		void HandleUpdate(const float dt);

		// ��ֹ��� �߰��Ѵ�.
		// pos = ��ֹ� ��ġ / radius = ��ֹ� ũ�� / height = ��ֹ� ������
		void AddTempObstacle(DirectX::XMFLOAT3 pos, float radius, float height);
		// pos = ��ֹ� ��ġ / bmin = ��ֹ� �ּ���ǥ / bmax = ��ֹ� �ִ���ǥ
		void AddBoxTempObstacle(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 bmin, DirectX::XMFLOAT3 bmax);
		// Ư�� ��ֹ��� �����Ѵ�.
		void RemoveTempObstacle(DirectX::XMFLOAT3 pos);
		// bpos�� �ִ� ��ֹ��� ��ġ�� npos�� ��ġ�� �ű��.
		void MoveTempObstacle(DirectX::XMFLOAT3 bpos, DirectX::XMFLOAT3 npos);
		// ��� ��ֹ��� �����Ѵ�.
		void ClearAllTempObstacles();

		// ������� Ž�� �Լ�
		std::vector<std::pair<DirectX::XMFLOAT3, DirectX::XMFLOAT3>> FindStraightPath(int index);
		// �̹� Ž���� ��θ� �������� �Լ�
		std::vector<std::pair<DirectX::XMFLOAT3, DirectX::XMFLOAT3>> GetPath(int index);
		// Raycast Ž�� �Լ�. (������ο� �ε����°� �ִٸ� �ű������ ��� ǥ��
		DirectX::XMFLOAT3 FindRaycastPath(int index);

		// startpos �� endpos�� �Է��ϴ� �Լ�. float[3] ����
		void SetSEpos(int index, float sx, float sy, float sz, float ex, float ey, float ez);
		// startpos �� endpos�� �Է��ϴ� �Լ�. XMFLOAT3 ����
		void SetSEpos(int index, DirectX::XMFLOAT3 startPosition, DirectX::XMFLOAT3 endPosition);
		// startpos�� �Է��ϴ� �Լ�. float[3] ����
		void SetStartpos(int index, float x, float y, float z);
		// startpos�� �Է��ϴ� �Լ�. XMFLOAT3 ����
		void SetStartpos(int index, DirectX::XMFLOAT3 position);
		// endpos�� �Է��ϴ� �Լ�. float[3] ����
		void SetEndpos(int index, float x, float y, float z);
		// endpos�� �Է��ϴ� �Լ�. XMFLOAT3 ����
		void SetEndpos(int index, DirectX::XMFLOAT3 position);
		// �׺�Ž��� �����ϱ� ���� agent�� �����ϴ� �Լ�. �� �������� ����
		void SetAgent(int index, float agentHeight, float agentMaxSlope, float agentRadius, float agentMaxClimb);
		
		// ��ǥ���� �Է��ؼ� �� ���� �׺�Ž� ���� �ִ��� �Ǵ��ϴ� �Լ��� 
		unsigned int GetPositionRef(int index, float x, float y, float z);	// Ref���� ��ȯ
		bool GetPositionOnMap(int index, float x, float y, float z);// �ܼ��� �׺�Ž� ���� �ִ��� �ƴ����� ��ȯ

		// �׺�Ž��� �����ϱ� ���� vertex�� index�� �������ִ� �Լ�(����)
		void GetNavmeshRenderInfo(int index, std::vector<DirectX::XMFLOAT3>& vertices, std::vector<unsigned int>& indices);
		// ��Ű��(�׺�Ž� �迭) ������ ��������
		int GetPackageSize();
		// ���� ������Ʈ ������ ��ȯ
		void GetAgent(int index, float& agentHeight, float& agentMaxSlope, float& agentRadius, float& agentMaxClimb);
		// ���� �׺�Ž� ���� �� ����Ǿ� �ִ� �׺�Ž����� ������
		std::vector<std::string> GetNavimeshPathList();

		std::vector<std::string> GetMapObjPathList();
	
	private:	// �̰Ŵ� private���!!!!! ������ ���ٰ� �ҹ��ڷ� �����ϴ� �Լ� ���̸� ���� �����!!!

		int rasterizeTileLayers(const int tx, const int ty, const rcConfig& cfg, struct TileCacheData* tiles, const int maxTiles);
		void getTilePos(const float* pos, int& tx, int& ty);
		void renderCachedTile(const int tx, const int ty, const int type);
		void renderCachedTileOverlay(const int tx, const int ty, double* proj, double* model, int* view);
		// sp, sq�� ������� ��ֹ��� ref ���� ��ȯ�ϴ� �Լ�. sp,sq�� ���� ���� ���� �𸣰���
		dtObstacleRef hitTestObstacle(const dtTileCache* tc, const float* sq);
		static bool isectSegAABB(const float* sq,const float* amin, const float* amax,float& tmin, float& tmax);
		
		bool compareXMFLOAT3(DirectX::XMFLOAT3 v1, DirectX::XMFLOAT3 v2);
		// float*�� XMFLOAT3�� ��ȯ ���ִ� ©�� �Լ� 
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

		// solo_mesh -> temp+obstacle�� �ٲٸ鼭 �߰��� �κ�
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