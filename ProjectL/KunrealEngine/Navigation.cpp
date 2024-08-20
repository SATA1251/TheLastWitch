//#include <math.h>
#include <stdio.h>
#include <string.h>
#include <filesystem>

#include "Navigation.h"
#include "RecastDump.h"
#include "DetourCommon.h"
#include "fastlz.h"

#pragma warning(disable:4996)

namespace KunrealEngine
{
	static int calcLayerBufferSize(const int gridWidth, const int gridHeight)
	{
		const int headerSize = dtAlign4(sizeof(dtTileCacheLayerHeader));
		const int gridSize = gridWidth * gridHeight;
		return headerSize + gridSize * 4;
	}

	PathFindPack::PathFindPack()
		: _startRef(0), _endRef(0), _startPos(), _endPos(), _filter(), _path(), _pathCount(0), _polyPickExt()
		, _navMesh(nullptr), _navQuery(nullptr), _tileCache(nullptr), _parent()
		// Straight-pathfind를 위해 필요한 부분들
		, _straightPath(), _straightPathFlags(), _straightPathPolys(), _nstraightPath(0)
		// Raycast-pathFind를 위해 필요한 부분들
		, _RaycastPathPolys(), _hit(), _hitPos()
	{
		_filter.setIncludeFlags(SAMPLE_POLYFLAGS_ALL ^ SAMPLE_POLYFLAGS_DISABLED);
		_filter.setExcludeFlags(0);

		_polyPickExt[0] = 2;
		_polyPickExt[1] = 4;
		_polyPickExt[2] = 2;

		_navQuery = new dtNavMeshQuery();
	}

	PathFindPack::~PathFindPack()
	{
		delete _navMesh;
		delete _navQuery;
		_navMesh = nullptr;
		_navQuery = nullptr;
	}

	// 추측하기로 대강의 구조?
	// rcCongig = 그냥 메쉬 어떻게 빌드 할건지에 대한 세팅

	Navigation::Navigation()
		// 객체 포인터
		:_geom(nullptr)
		// 결과 빌드타임등
		, _keepInterResults(false), _totalBuildTimeMs(0.0f)
		// 빌드정보
		, _triareas(nullptr), _solid(nullptr), _chf(nullptr), _cset(nullptr), _pmesh(nullptr)
		, _cfg(), _dmesh(nullptr), _ctx(nullptr)
		//
		, _navMeshDrawFlags('\x3')
		// 빌드세팅
		, _cellSize(0.3), _cellHeight(0.2), _regionMinSize(8.0), _regionMergeSize(20.0), _edgeMaxLen(12.0), _edgeMaxError(1.3)
		, _vertsPerPoly(3.0), _detailSampleDist(6.0), _detailSampleMaxError(1), _partitionType(0)
		// 필터 정보
		, _filterLowHangingObstacles(false), _filterLedgeSpans(false), _filterWalkableLowHeightSpans(false)
		// Obstacle에서 추가 된 부분
		, _maxTiles(1024), _maxPolysPerTile(4096)
		// 그외
		, _cacheBuildMemUsage(0), _cacheBuildTimeMs(0), _cacheCompressedSize(0), _cacheRawSize(0), _tmproc(nullptr)
		, _tcomp(nullptr), _talloc(nullptr), _cacheLayerCount(0)
	{

	}

	Navigation::~Navigation()
	{

	}

	Navigation& Navigation::GetInstance()
	{
		static Navigation* instance;

		if (instance == nullptr)
		{
			instance = new Navigation();
		}
		return *instance;
	}

	void Navigation::Initialize()
	{
		_geom = new class InputGeom();
		_ctx = new rcContext();
		_talloc = new LinearAllocator(32000);
		_tcomp = new FastLZCompressor();
		_tmproc = new MeshProcess();
	}

	void Navigation::Release()
	{

	}

	void Navigation::LoadAll(const char* path, int index)
	{
		FILE* fp = fopen(path, "rb");
		if (!fp) return;

		// Read header.
		TileCacheSetHeader header;
		size_t headerReadReturnCode = fread(&header, sizeof(TileCacheSetHeader), 1, fp);
		if (headerReadReturnCode != 1)
		{
			// Error or early EOF
			fclose(fp);
			return;
		}
		if (header.magic != TILECACHESET_MAGIC)
		{
			fclose(fp);
			return;
		}
		if (header.version != TILECACHESET_VERSION)
		{
			fclose(fp);
			return;
		}

		_package[index]._navMesh = dtAllocNavMesh();
		if (!_package[index]._navMesh)
		{
			fclose(fp);
			return;
		}
		dtStatus status = _package[index]._navMesh->init(&header.meshParams);
		if (dtStatusFailed(status))
		{
			fclose(fp);
			return;
		}

		_package[index]._tileCache = dtAllocTileCache();
		if (!_package[index]._tileCache)
		{
			fclose(fp);
			return;
		}
		status = _package[index]._tileCache->init(&header.cacheParams, _talloc, _tcomp, _tmproc);
		if (dtStatusFailed(status))
		{
			fclose(fp);
			return;
		}

		// Read tiles.
		for (int i = 0; i < header.numTiles; ++i)
		{
			TileCacheTileHeader tileHeader;
			size_t tileHeaderReadReturnCode = fread(&tileHeader, sizeof(tileHeader), 1, fp);
			if (tileHeaderReadReturnCode != 1)
			{
				// Error or early EOF
				fclose(fp);
				return;
			}
			if (!tileHeader.tileRef || !tileHeader.dataSize)
				break;

			unsigned char* data = (unsigned char*)dtAlloc(tileHeader.dataSize, DT_ALLOC_PERM);
			if (!data) break;
			memset(data, 0, tileHeader.dataSize);
			size_t tileDataReadReturnCode = fread(data, tileHeader.dataSize, 1, fp);
			if (tileDataReadReturnCode != 1)
			{
				// Error or early EOF
				dtFree(data);
				fclose(fp);
				return;
			}

			dtCompressedTileRef tile = 0;
			dtStatus addTileStatus = _package[index]._tileCache->addTile(data, tileHeader.dataSize, DT_COMPRESSEDTILE_FREE_DATA, &tile);
			if (dtStatusFailed(addTileStatus))
			{
				dtFree(data);
			}

			if (tile)
				_package[index]._tileCache->buildNavMeshTile(tile, _package[index]._navMesh);
		}

		fclose(fp);

		_package[index]._navQuery->init(_package[index]._navMesh, 2048);
		return;
	}

	void Navigation::SaveAll(int index, const char* path)
	{
		if (!_package[index]._tileCache) return;

		FILE* fp = fopen(path, "wb");
		if (!fp)
			return;

		// Store header.
		TileCacheSetHeader header;
		header.magic = TILECACHESET_MAGIC;
		header.version = TILECACHESET_VERSION;
		header.numTiles = 0;
		for (int i = 0 ; i < _package[index]._tileCache->getTileCount() ; ++i)
		{
			const dtCompressedTile* tile = _package[index]._tileCache->getTile(i);
			if (!tile || !tile->header || !tile->dataSize) continue;
			header.numTiles++;
		}
		memcpy(&header.cacheParams, _package[index]._tileCache->getParams(), sizeof(dtTileCacheParams));
		memcpy(&header.meshParams, _package[index]._navMesh->getParams(), sizeof(dtNavMeshParams));
		fwrite(&header, sizeof(TileCacheSetHeader), 1, fp);

		// Store tiles.
		for (int i = 0; i < _package[index]._tileCache->getTileCount(); ++i)
		{
			const dtCompressedTile* tile = _package[index]._tileCache->getTile(i);
			if (!tile || !tile->header || !tile->dataSize) continue;

			TileCacheTileHeader tileHeader;
			tileHeader.tileRef = _package[index]._tileCache->getTileRef(tile);
			tileHeader.dataSize = tile->dataSize;
			fwrite(&tileHeader, sizeof(tileHeader), 1, fp);

			fwrite(tile->data, tile->dataSize, 1, fp);
		}

		fclose(fp);
	}

	bool Navigation::HandleBuild(int index, std::string path)
	{
		path = "Resources/MapObj/" + path;

		dtStatus status;
		_geom->load(_ctx, path);

		if (!_geom || !_geom->getMesh())
		{
			_ctx->log(RC_LOG_ERROR, "buildTiledNavigation: No vertices and triangles.");
			return false;
		}

		_tmproc->init(_geom);

		// Init cache
		const float* bmin = _geom->getNavMeshBoundsMin();
		const float* bmax = _geom->getNavMeshBoundsMax();
		int gw = 0, gh = 0;
		rcCalcGridSize(bmin, bmax, _cellSize, &gw, &gh);
		const int ts = (int)_tileSize;
		const int tw = (gw + ts - 1) / ts;
		const int th = (gh + ts - 1) / ts;

		// Generation params.
		rcConfig cfg;
		memset(&cfg, 0, sizeof(cfg));
		cfg.cs = _cellSize;
		cfg.ch = _cellHeight;
		cfg.walkableSlopeAngle = _package[index]._agentsetting._agentMaxSlope;
		cfg.walkableHeight = (int)ceilf(_package[index]._agentsetting._agentHeight / cfg.ch);
		cfg.walkableClimb = (int)floorf(_package[index]._agentsetting._agentMaxClimb / cfg.ch);
		cfg.walkableRadius = (int)ceilf(_package[index]._agentsetting._agentRadius / cfg.cs);
		cfg.maxEdgeLen = (int)(_edgeMaxLen / _cellSize);
		cfg.maxSimplificationError = _edgeMaxError;
		cfg.minRegionArea = (int)rcSqr(_regionMinSize);		// Note: area = size*size
		cfg.mergeRegionArea = (int)rcSqr(_regionMergeSize);	// Note: area = size*size
		cfg.maxVertsPerPoly = (int)_vertsPerPoly;
		cfg.tileSize = (int)_tileSize;
		cfg.borderSize = cfg.walkableRadius + 3; // Reserve enough padding.
		cfg.width = cfg.tileSize + cfg.borderSize * 2;
		cfg.height = cfg.tileSize + cfg.borderSize * 2;
		cfg.detailSampleDist = _detailSampleDist < 0.9f ? 0 : _cellSize * _detailSampleDist;
		cfg.detailSampleMaxError = _cellHeight * _detailSampleMaxError;
		rcVcopy(cfg.bmin, bmin);
		rcVcopy(cfg.bmax, bmax);

		// Tile cache params.
		dtTileCacheParams tcparams;
		memset(&tcparams, 0, sizeof(tcparams));
		rcVcopy(tcparams.orig, bmin);
		tcparams.cs = _cellSize;
		tcparams.ch = _cellHeight;
		tcparams.width = (int)_tileSize;
		tcparams.height = (int)_tileSize;
		tcparams.walkableHeight = _package[index]._agentsetting._agentHeight;
		tcparams.walkableRadius = _package[index]._agentsetting._agentRadius;
		tcparams.walkableClimb = _package[index]._agentsetting._agentMaxClimb;
		tcparams.maxSimplificationError = _edgeMaxError;
		tcparams.maxTiles = tw * th * EXPECTED_LAYERS_PER_TILE;
		tcparams.maxObstacles = 128;

		dtFreeTileCache(_package[index]._tileCache);

		_package[index]._tileCache = dtAllocTileCache();
		if (!_package[index]._tileCache)
		{
			_ctx->log(RC_LOG_ERROR, "buildTiledNavigation: Could not allocate tile cache.");
			return false;
		}
		status = _package[index]._tileCache->init(&tcparams, _talloc, _tcomp, _tmproc);
		if (dtStatusFailed(status))
		{
			_ctx->log(RC_LOG_ERROR, "buildTiledNavigation: Could not init tile cache.");
			return false;
		}

		dtFreeNavMesh(_package[index]._navMesh);

		_package[index]._navMesh = dtAllocNavMesh();
		if (!_package[index]._navMesh)
		{
			_ctx->log(RC_LOG_ERROR, "buildTiledNavigation: Could not allocate navmesh.");
			return false;
		}

		dtNavMeshParams params;
		memset(&params, 0, sizeof(params));
		rcVcopy(params.orig, bmin);
		params.tileWidth = _tileSize * _cellSize;
		params.tileHeight = _tileSize * _cellSize;
		params.maxTiles = _maxTiles;
		params.maxPolys = _maxPolysPerTile;

		status = _package[index]._navMesh->init(&params);
		if (dtStatusFailed(status))
		{
			_ctx->log(RC_LOG_ERROR, "buildTiledNavigation: Could not init navmesh.");
			return false;
		}

		status = _package[index]._navQuery->init(_package[index]._navMesh, 2048);
		if (dtStatusFailed(status))
		{
			_ctx->log(RC_LOG_ERROR, "buildTiledNavigation: Could not init Detour navmesh query");
			return false;
		}


		// Preprocess tiles.

		_ctx->resetTimers();

		_cacheLayerCount = 0;
		_cacheCompressedSize = 0;
		_cacheRawSize = 0;

		for (int y = 0; y < th; ++y)
		{
			for (int x = 0; x < tw; ++x)
			{
				TileCacheData tiles[MAX_LAYERS];
				memset(tiles, 0, sizeof(tiles));
				int ntiles = rasterizeTileLayers(x, y, cfg, tiles, MAX_LAYERS);

				for (int i = 0; i < ntiles; ++i)
				{
					TileCacheData* tile = &tiles[i];
					status = _package[index]._tileCache->addTile(tile->data, tile->dataSize, DT_COMPRESSEDTILE_FREE_DATA, 0);
					if (dtStatusFailed(status))
					{
						dtFree(tile->data);
						tile->data = 0;
						continue;
					}

					_cacheLayerCount++;
					_cacheCompressedSize += tile->dataSize;
					_cacheRawSize += calcLayerBufferSize(tcparams.width, tcparams.height);
				}
			}
		}

		// Build initial meshes
		_ctx->startTimer(RC_TIMER_TOTAL);
		for (int y = 0; y < th; ++y)
		{

			for (int x = 0; x < tw; ++x)
			{
				_package[index]._tileCache->buildNavMeshTilesAt(x, y, _package[index]._navMesh);
			}
		}

		const dtNavMesh* nav = _package[index]._navMesh;
		int navmeshMemUsage = 0;
		for (int i = 0; i < nav->getMaxTiles(); ++i)
		{
			const dtMeshTile* tile = nav->getTile(i);
			if (tile->header)
				navmeshMemUsage += tile->dataSize;
		}
		return true;
	}

	void Navigation::HandleUpdate(const float dt)
	{
		for (int i = 0; i < PACKAGESIZE; i++)
		{
			if (!_package[i]._navMesh)
				return;
			if (!_package[i]._tileCache)
				return;
			_package[i]._tileCache->update(dt, _package[i]._navMesh);
		}
	}

	void Navigation::GetNavmeshRenderInfo(int index, std::vector<DirectX::XMFLOAT3>& vertices, std::vector<unsigned int>& indices)
	{
		const dtNavMesh* navmesh = _package[index]._navMesh;
		for (int i = 0; i < navmesh->getMaxTiles(); ++i)
		{
			const dtMeshTile* tile = navmesh->getTile(i);
			if (tile->header == nullptr)
			{
				break;
			}

			for (int j = 0; j < tile->header->polyCount; ++j)
			{
				const dtPoly* p = &tile->polys[j];
				const dtPolyDetail* pd = &tile->detailMeshes[j];

				for (int k = 0; k < pd->triCount; ++k)
				{
					const unsigned char* t = &tile->detailTris[(pd->triBase + k) * 4];
					for (int y = 0; y < 3; ++y)
					{
						if (t[y] < p->vertCount)
						{
							DirectX::XMFLOAT3 vert(vertex(&tile->verts[p->verts[t[y]] * 3]));
							vertices.push_back(vert);
						}
						else
						{
							DirectX::XMFLOAT3 vert(vertex(&tile->detailVerts[(pd->vertBase + t[y] - p->vertCount) * 3]));
							vertices.push_back(vert);
						}
					}
				}
			}
		}

		for (unsigned int i = 1; i < vertices.size(); i += 3)
		{
			indices.push_back(i - 1);
			indices.push_back(i);
			indices.push_back(i + 1);
		}
	}

	int Navigation::GetPackageSize()
	{
		return PACKAGESIZE;
	}

	void Navigation::GetAgent(int index, float& agentHeight, float& agentMaxSlope, float& agentRadius, float& agentMaxClimb)
	{
		agentHeight = _package[index]._agentsetting._agentHeight;
		agentMaxSlope =	_package[index]._agentsetting._agentMaxSlope;
		agentRadius = _package[index]._agentsetting._agentMaxClimb;
		agentMaxClimb =	_package[index]._agentsetting._agentRadius;
	}

	std::vector<std::string> KunrealEngine::Navigation::GetNavimeshPathList()
	{
		namespace fs = std::filesystem;
		std::string directory = "Resources/Navimesh";
		std::vector<std::string> fileNames;

		// Iterate over the files in the directory
		for (const auto& entry : fs::directory_iterator(directory)) {
			// Check if it's a regular file
			if (fs::is_regular_file(entry)) {
				// Get the filename from the path and add it to the vector
				fileNames.push_back(entry.path().filename().string());
			}
		}

		return fileNames;
	}

	std::vector<std::string> KunrealEngine::Navigation::GetMapObjPathList()
	{
		namespace fs = std::filesystem;
		std::string directory = "Resources/MapObj";
		std::vector<std::string> fileNames;

		// Iterate over the files in the directory
		for (const auto& entry : fs::directory_iterator(directory)) {
			// Check if it's a regular file
			if (fs::is_regular_file(entry)) {
				// Get the filename from the path and add it to the vector
				fileNames.push_back(entry.path().filename().string());
			}
		}

		return fileNames;
	}

	int Navigation::rasterizeTileLayers(const int tx, const int ty, const rcConfig& cfg, TileCacheData* tiles, const int maxTiles)
	{
		if (!_geom || !_geom->getMesh() || !_geom->getChunkyMesh())
		{
			_ctx->log(RC_LOG_ERROR, "buildTile: Input mesh is not specified.");
			return 0;
		}

		FastLZCompressor comp;
		RasterizationContext rc;

		const float* verts = _geom->getMesh()->getVerts();
		const int nverts = _geom->getMesh()->getVertCount();
		const rcChunkyTriMesh* chunkyMesh = _geom->getChunkyMesh();

		// Tile bounds.
		const float tcs = cfg.tileSize * cfg.cs;

		rcConfig tcfg;
		memcpy(&tcfg, &cfg, sizeof(tcfg));

		tcfg.bmin[0] = cfg.bmin[0] + tx * tcs;
		tcfg.bmin[1] = cfg.bmin[1];
		tcfg.bmin[2] = cfg.bmin[2] + ty * tcs;
		tcfg.bmax[0] = cfg.bmin[0] + (tx + 1) * tcs;
		tcfg.bmax[1] = cfg.bmax[1];
		tcfg.bmax[2] = cfg.bmin[2] + (ty + 1) * tcs;
		tcfg.bmin[0] -= tcfg.borderSize * tcfg.cs;
		tcfg.bmin[2] -= tcfg.borderSize * tcfg.cs;
		tcfg.bmax[0] += tcfg.borderSize * tcfg.cs;
		tcfg.bmax[2] += tcfg.borderSize * tcfg.cs;

		// Allocate voxel heightfield where we rasterize our input data to.
		rc.solid = rcAllocHeightfield();
		if (!rc.solid)
		{
			_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'solid'.");
			return 0;
		}
		if (!rcCreateHeightfield(_ctx, *rc.solid, tcfg.width, tcfg.height, tcfg.bmin, tcfg.bmax, tcfg.cs, tcfg.ch))
		{
			_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not create solid heightfield.");
			return 0;
		}

		// Allocate array that can hold triangle flags.
		// If you have multiple meshes you need to process, allocate
		// and array which can hold the max number of triangles you need to process.
		rc.triareas = new unsigned char[chunkyMesh->maxTrisPerChunk];
		if (!rc.triareas)
		{
			_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'm_triareas' (%d).", chunkyMesh->maxTrisPerChunk);
			return 0;
		}

		float tbmin[2], tbmax[2];
		tbmin[0] = tcfg.bmin[0];
		tbmin[1] = tcfg.bmin[2];
		tbmax[0] = tcfg.bmax[0];
		tbmax[1] = tcfg.bmax[2];
		int cid[512];// TODO: Make grow when returning too many items.
		const int ncid = rcGetChunksOverlappingRect(chunkyMesh, tbmin, tbmax, cid, 512);
		if (!ncid)
		{
			return 0; // empty
		}

		for (int i = 0; i < ncid; ++i)
		{
			const rcChunkyTriMeshNode& node = chunkyMesh->nodes[cid[i]];
			const int* tris = &chunkyMesh->tris[node.i * 3];
			const int ntris = node.n;

			memset(rc.triareas, 0, ntris * sizeof(unsigned char));
			rcMarkWalkableTriangles(_ctx, tcfg.walkableSlopeAngle,
				verts, nverts, tris, ntris, rc.triareas);

			if (!rcRasterizeTriangles(_ctx, verts, nverts, tris, rc.triareas, ntris, *rc.solid, tcfg.walkableClimb))
				return 0;
		}

		// Once all geometry is rasterized, we do initial pass of filtering to
		// remove unwanted overhangs caused by the conservative rasterization
		// as well as filter spans where the character cannot possibly stand.
		if (_filterLowHangingObstacles)
			rcFilterLowHangingWalkableObstacles(_ctx, tcfg.walkableClimb, *rc.solid);
		if (_filterLedgeSpans)
			rcFilterLedgeSpans(_ctx, tcfg.walkableHeight, tcfg.walkableClimb, *rc.solid);
		if (_filterWalkableLowHeightSpans)
			rcFilterWalkableLowHeightSpans(_ctx, tcfg.walkableHeight, *rc.solid);


		rc.chf = rcAllocCompactHeightfield();
		if (!rc.chf)
		{
			_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'chf'.");
			return 0;
		}
		if (!rcBuildCompactHeightfield(_ctx, tcfg.walkableHeight, tcfg.walkableClimb, *rc.solid, *rc.chf))
		{
			_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build compact data.");
			return 0;
		}

		// Erode the walkable area by agent radius.
		if (!rcErodeWalkableArea(_ctx, tcfg.walkableRadius, *rc.chf))
		{
			_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not erode.");
			return 0;
		}

		// (Optional) Mark areas.
		const ConvexVolume* vols = _geom->getConvexVolumes();
		for (int i = 0; i < _geom->getConvexVolumeCount(); ++i)
		{
			rcMarkConvexPolyArea(_ctx, vols[i].verts, vols[i].nverts,
				vols[i].hmin, vols[i].hmax,
				(unsigned char)vols[i].area, *rc.chf);
		}

		rc.lset = rcAllocHeightfieldLayerSet();
		if (!rc.lset)
		{
			_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'lset'.");
			return 0;
		}
		if (!rcBuildHeightfieldLayers(_ctx, *rc.chf, tcfg.borderSize, tcfg.walkableHeight, *rc.lset))
		{
			_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build heighfield layers.");
			return 0;
		}

		rc.ntiles = 0;
		for (int i = 0; i < rcMin(rc.lset->nlayers, MAX_LAYERS); ++i)
		{
			TileCacheData* tile = &rc.tiles[rc.ntiles++];
			const rcHeightfieldLayer* layer = &rc.lset->layers[i];

			// Store header
			dtTileCacheLayerHeader header;
			header.magic = DT_TILECACHE_MAGIC;
			header.version = DT_TILECACHE_VERSION;

			// Tile layer location in the navmesh.
			header.tx = tx;
			header.ty = ty;
			header.tlayer = i;
			dtVcopy(header.bmin, layer->bmin);
			dtVcopy(header.bmax, layer->bmax);

			// Tile info.
			header.width = (unsigned char)layer->width;
			header.height = (unsigned char)layer->height;
			header.minx = (unsigned char)layer->minx;
			header.maxx = (unsigned char)layer->maxx;
			header.miny = (unsigned char)layer->miny;
			header.maxy = (unsigned char)layer->maxy;
			header.hmin = (unsigned short)layer->hmin;
			header.hmax = (unsigned short)layer->hmax;

			dtStatus status = dtBuildTileCacheLayer(&comp, &header, layer->heights, layer->areas, layer->cons,
				&tile->data, &tile->dataSize);
			if (dtStatusFailed(status))
			{
				return 0;
			}
		}

		// Transfer ownsership of tile data from build context to the caller.
		int n = 0;
		for (int i = 0; i < rcMin(rc.ntiles, maxTiles); ++i)
		{
			tiles[n++] = rc.tiles[i];
			rc.tiles[i].data = 0;
			rc.tiles[i].dataSize = 0;
		}

		return n;
	}

	/// END HANDLE BUILD

	void Navigation::getTilePos(const float* pos, int& tx, int& ty)
	{
		if (!_geom) return;

		const float* bmin = _geom->getNavMeshBoundsMin();

		const float ts = _tileSize * _cellSize;
		tx = (int)((pos[0] - bmin[0]) / ts);
		ty = (int)((pos[2] - bmin[2]) / ts);
	}

	void Navigation::renderCachedTile(const int tx, const int ty, const int type)
	{
		// 		if (_tileCache)
		// 			drawDetail(&m_dd, m_tileCache, tx, ty, type);
	}

	void Navigation::renderCachedTileOverlay(const int tx, const int ty, double* proj, double* model, int* view)
	{
		// 		if (m_tileCache)
		// 			drawDetailOverlay(m_tileCache, tx, ty, proj, model, view);
	}

	void Navigation::AddTempObstacle(DirectX::XMFLOAT3 pos, float radius, float height)
	{
		for (int i = 0; i < PACKAGESIZE; i++)
		{
			if (!_package[i]._tileCache)
				return;

			float p[3];
			p[0] = pos.x;
			p[1] = pos.y - 0.5f;
			p[2] = pos.z;

			_package[i]._tileCache->addObstacle(p, radius, height, 0);
		}
	}

	void Navigation::AddBoxTempObstacle(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 bmin, DirectX::XMFLOAT3 bmax)
	{
			float p[3];
			p[0] = bmin.x - pos.x;
			p[1] = bmin.y - pos.y;
			p[2] = bmin.z - pos.z;

			float p1[3];
			p1[0] = bmax.x + pos.x;
			p1[1] = bmax.y + pos.y;
			p1[2] = bmax.z + pos.z;

		for (int i = 0; i < PACKAGESIZE; i++)
		{
			if (!_package[i]._tileCache)
				return;


			_package[i]._tileCache->addBoxObstacle(p, p1, 0);
		}
	}

	void Navigation::RemoveTempObstacle(DirectX::XMFLOAT3 pos)
	{
		for (int i = 0; i < PACKAGESIZE; i++)
		{
			if (!_package[i]._tileCache)
				return;

			float p[3];
			p[0] = pos.x;
			p[1] = pos.y;
			p[2] = pos.z;

			dtObstacleRef ref = hitTestObstacle(_package[i]._tileCache, p);
			_package[i]._tileCache->removeObstacle(ref);
		}
	}

	void Navigation::MoveTempObstacle(DirectX::XMFLOAT3 bpos, DirectX::XMFLOAT3 npos)
	{
		if (!_package[0]._tileCache)
			return;

		float p1[3];
		p1[0] = bpos.x;
		p1[1] = bpos.y;
		p1[2] = bpos.z;

		dtObstacleRef ref = hitTestObstacle(_package[0]._tileCache, p1);
		_package[0]._tileCache->removeObstacle(ref);

		if (!_package[0]._tileCache)
			return;

		float p2[3];
		p2[0] = npos.x;
		p2[1] = npos.y - 0.5f;
		p2[2] = npos.z;

		_package[0]._tileCache->addObstacle(p2, 5, 5, 0);
	}

	void Navigation::ClearAllTempObstacles()
	{
		for (int i = 0; i < PACKAGESIZE; i++)
		{
			if (!_package[i]._tileCache)
				return;
			for (int f = 0; f < _package[i]._tileCache->getObstacleCount(); ++f)
			{
				const dtTileCacheObstacle* ob = _package[i]._tileCache->getObstacle(f);
				if (ob->state == DT_OBSTACLE_EMPTY) continue;
				_package[i]._tileCache->removeObstacle(_package[0]._tileCache->getObstacleRef(ob));
			}
		}
	}

	std::vector<std::pair<DirectX::XMFLOAT3, DirectX::XMFLOAT3>> Navigation::FindStraightPath(int index)
	{
#define PACKAGE _package[index]

		std::vector<std::pair<DirectX::XMFLOAT3, DirectX::XMFLOAT3>> failContainer;
		if (!PACKAGE._navMesh || !PACKAGE._navQuery)
			return failContainer;

		PACKAGE._navQuery->findPath(PACKAGE._startRef, PACKAGE._endRef, PACKAGE._startPos, PACKAGE._endPos, &(PACKAGE._filter)
			, PACKAGE._path, &(PACKAGE._pathCount), PACKAGE.MAX_POLYS);
		PACKAGE._navQuery->findStraightPath(PACKAGE._startPos, PACKAGE._endPos, PACKAGE._path, PACKAGE._pathCount, PACKAGE._straightPath
			, PACKAGE._straightPathFlags, PACKAGE._straightPathPolys, &(PACKAGE._nstraightPath)
			, PACKAGE.MAX_POLYS, DT_STRAIGHTPATH_ALL_CROSSINGS);

		return GetPath(index);
	}

	std::vector<std::pair<DirectX::XMFLOAT3, DirectX::XMFLOAT3>> Navigation::GetPath(int index)
	{
		std::vector<std::pair<DirectX::XMFLOAT3, DirectX::XMFLOAT3>> result;

		for (int i = 0; i < _package[index]._nstraightPath - 1; ++i)
		{
			DirectX::XMFLOAT3 strindex;
			DirectX::XMFLOAT3 nxtindex;

			strindex.x = _package[index]._straightPath[i * 3];
			strindex.y = _package[index]._straightPath[i * 3 + 1] + 0.4f;
			strindex.z = _package[index]._straightPath[i * 3 + 2];

			nxtindex.x = _package[index]._straightPath[(i + 1) * 3];
			nxtindex.y = _package[index]._straightPath[(i + 1) * 3 + 1] + 0.4f;
			nxtindex.z = _package[index]._straightPath[(i + 1) * 3 + 2];

			result.push_back(std::make_pair(strindex, nxtindex));
		}

		return result;
	}

	DirectX::XMFLOAT3 Navigation::FindRaycastPath(int index)
	{
#define PACKAGE _package[index]

		DirectX::XMFLOAT3 result = {};

		float t = 0;
		PACKAGE._pathCount = 0;
		PACKAGE._nstraightPath = 2;
		PACKAGE._straightPath[0] = PACKAGE._startPos[0];
		PACKAGE._straightPath[1] = PACKAGE._startPos[1];
		PACKAGE._straightPath[2] = PACKAGE._startPos[2];
		dtStatus status = PACKAGE._navQuery->raycast(PACKAGE._startRef, PACKAGE._startPos, PACKAGE._endPos
			, &PACKAGE._filter, DT_RAYCAST_USE_COSTS
			, &PACKAGE._hit, PACKAGE._RaycastPathPolys);
		if (PACKAGE._hit.t > 1)
		{
			// No hit
			dtVcopy(PACKAGE._hitPos, PACKAGE._endPos);
		}
		else
		{
			// Hit
			dtVlerp(PACKAGE._hitPos, PACKAGE._startPos, PACKAGE._endPos, PACKAGE._hit.t);
		}
		// Adjust height.
		if (PACKAGE._pathCount > 0)
		{
			float h = 0;
			PACKAGE._navQuery->getPolyHeight(PACKAGE._path[PACKAGE._pathCount - 1], PACKAGE._hitPos, &h);
			PACKAGE._hitPos[1] = h;
		}
		result.x = PACKAGE._hitPos[0];
		result.y = PACKAGE._hitPos[1];
		result.z = PACKAGE._hitPos[2];

		return result;
	}

	void Navigation::SetSEpos(int index, float sx, float sy, float sz, float ex, float ey, float ez)
	{
		_package[index]._startPos[0] = sx;
		_package[index]._startPos[1] = sy;
		_package[index]._startPos[2] = sz;
		_package[index]._navQuery->findNearestPoly(_package[index]._startPos, _package[index]._polyPickExt, &(_package[index]._filter), &(_package[index]._startRef), 0);

		_package[index]._endPos[0] = ex;
		_package[index]._endPos[1] = ey;
		_package[index]._endPos[2] = ez;
		_package[index]._navQuery->findNearestPoly(_package[index]._endPos, _package[index]._polyPickExt, &(_package[index]._filter), &(_package[index]._endRef), 0);
	}

	void Navigation::SetSEpos(int index, DirectX::XMFLOAT3 startPosition, DirectX::XMFLOAT3 endPosition)
	{
		SetSEpos(index, startPosition.x, startPosition.y, startPosition.z
			, endPosition.x, endPosition.y, endPosition.z);
	}

	void Navigation::SetStartpos(int index, float x, float y, float z)
	{
		_package[index]._startPos[0] = x;
		_package[index]._startPos[1] = y;
		_package[index]._startPos[2] = z;
		_package[index]._navQuery->findNearestPoly(_package[index]._startPos, _package[index]._polyPickExt, &(_package[index]._filter), &(_package[index]._startRef), 0);
	}

	void Navigation::SetStartpos(int index, DirectX::XMFLOAT3 position)
	{
		SetStartpos(index, position.x, position.y, position.z);
	}

	void Navigation::SetEndpos(int index, float x, float y, float z)
	{
		_package[index]._endPos[0] = x;
		_package[index]._endPos[1] = y;
		_package[index]._endPos[2] = z;
		_package[index]._navQuery->findNearestPoly(_package[index]._endPos, _package[index]._polyPickExt, &(_package[index]._filter), &(_package[index]._endRef), 0);
	}

	void Navigation::SetEndpos(int index, DirectX::XMFLOAT3 position)
	{
		SetEndpos(index, position.x, position.y, position.z);
	}

	void Navigation::SetAgent(int index, float agentHeight, float agentMaxSlope, float agentRadius, float agentMaxClimb)
	{
		_package[index]._agentsetting._agentHeight = agentHeight;
		_package[index]._agentsetting._agentMaxSlope = agentMaxSlope;
		_package[index]._agentsetting._agentRadius = agentRadius;
		_package[index]._agentsetting._agentMaxClimb = agentMaxClimb;
	}

	unsigned int Navigation::GetPositionRef(int index, float x, float y, float z)
	{
		float pos[3] = { x, y, z };
		dtPolyRef ref = 0;
		_package[index]._navQuery->findNearestPoly(pos, _package[index]._polyPickExt, &(_package[index]._filter), &ref, 0);
		return (unsigned int)ref;
	}

	bool Navigation::GetPositionOnMap(int index, float x, float y, float z)
	{
		float pos[3] = { x, y, z };
		dtPolyRef ref = 0;
		_package[index]._navQuery->findNearestPoly(pos, _package[index]._polyPickExt, &(_package[index]._filter), &ref, 0);
		
		if (ref > 0)
		{
			return true;
		}

		else
		{
			return false;
		}
	}

	dtObstacleRef Navigation::hitTestObstacle(const dtTileCache* tc, const float* sq)
	{
		float tmin = FLT_MAX;
		const dtTileCacheObstacle* obmin = nullptr;
		for (int i = 0; i < tc->getObstacleCount(); ++i)
		{
			const dtTileCacheObstacle* ob = tc->getObstacle(i);
			if (ob->state == DT_OBSTACLE_EMPTY)
				continue;

			float bmin[3], bmax[3], t0, t1;
			tc->getObstacleBounds(ob, bmin, bmax);

			if (Navigation::isectSegAABB(sq, bmin, bmax, t0, t1))
			{
				if (t0 < tmin)
				{
					tmin = t0;
					obmin = ob;
				}
			}
		}
		return tc->getObstacleRef(obmin);
	}

	bool Navigation::isectSegAABB(const float* sq, const float* amin, const float* amax, float& tmin, float& tmax)
	{
		static const float EPS = 1e-6f;

		float d[3];
		float sp[3] = { 0.0f, 0.0f, 0.0f }; // Assuming the origin of the ray is at (0, 0, 0)
		rcVsub(d, sq, sp);
		tmin = 0;  // set to -FLT_MAX to get first hit on line
		tmax = FLT_MAX;        // set to max distance ray can travel (for segment)

		// For all three slabs
		for (int i = 0; i < 3; i++)
		{
			if (fabsf(d[i]) < EPS)
			{
				// Ray is parallel to slab. No hit if origin not within slab
				if (sp[i] < amin[i] || sp[i] > amax[i])
					return false;
			}
			else
			{
				// Compute intersection t value of ray with near and far plane of slab
				const float ood = 1.0f / d[i];
				float t1 = (amin[i] - sp[i]) * ood;
				float t2 = (amax[i] - sp[i]) * ood;
				// Make t1 be intersection with near plane, t2 with far plane
				if (t1 > t2) rcSwap(t1, t2);
				// Compute the intersection of slab intersections intervals
				if (t1 > tmin) tmin = t1;
				if (t2 < tmax) tmax = t2;
				// Exit with no collision as soon as slab intersection becomes empty
				if (tmin > tmax) return false;
			}
		}

		return true;
	}

	bool Navigation::compareXMFLOAT3(DirectX::XMFLOAT3 v1, DirectX::XMFLOAT3 v2)
	{
		bool result = true;
		result = v1.x == v2.x ? false : result;
		result = v1.y == v2.y ? false : result;
		result = v1.z == v2.z ? false : result;
		return false;
	}

	DirectX::XMFLOAT3 Navigation::vertex(const float* pos)
	{
		return DirectX::XMFLOAT3(pos[0], pos[1], pos[2]);
	}
}
