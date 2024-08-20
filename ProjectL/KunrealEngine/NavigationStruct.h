#pragma once
#include "Recast.h"
#include "fastlz.h"
#include "InputGeom.h"
#include "DetourNavMesh.h"
#include "ChunkyTriMesh.h"
#include "DetourNavMeshQuery.h"
#include "DetourNavMeshBuilder.h"
#include "DetourTileCache.h"
#include "DetourTileCacheBuilder.h"

namespace KunrealEngine
{
	enum SamplePolyAreas
	{
		SAMPLE_POLYAREA_GROUND,
		SAMPLE_POLYAREA_WATER,
		SAMPLE_POLYAREA_ROAD,
		SAMPLE_POLYAREA_DOOR,
		SAMPLE_POLYAREA_GRASS,
		SAMPLE_POLYAREA_JUMP
	};
	enum SamplePolyFlags
	{
		SAMPLE_POLYFLAGS_WALK = 0x01,		// 걷기 능력 (지상, 풀, 도로)
		SAMPLE_POLYFLAGS_SWIM = 0x02,		// 수영 능력 (물)
		SAMPLE_POLYFLAGS_DOOR = 0x04,		// 문 통과 능력
		SAMPLE_POLYFLAGS_JUMP = 0x08,		// 점프 능력
		SAMPLE_POLYFLAGS_DISABLED = 0x10,	// 비활성화된 폴리곤
		SAMPLE_POLYFLAGS_ALL = 0xffff		// 모든 능력
	};
	enum SamplePartitionType
	{
		SAMPLE_PARTITION_WATERSHED,
		SAMPLE_PARTITION_MONOTONE,
		SAMPLE_PARTITION_LAYERS
	};

	enum TestType
	{
		TEST_PATHFIND,
		TEST_RAYCAST
	};

	static const int NAVMESHSET_MAGIC = 'M' << 24 | 'S' << 16 | 'E' << 8 | 'T'; //'MSET';
	static const int NAVMESHSET_VERSION = 1;

	static const int TILECACHESET_MAGIC = 'T' << 24 | 'S' << 16 | 'E' << 8 | 'T'; //'TSET';
	static const int TILECACHESET_VERSION = 1;
	static const int EXPECTED_LAYERS_PER_TILE = 4;
	static const int MAX_LAYERS = 32;

	struct LinearAllocator : public dtTileCacheAlloc
	{
		unsigned char* buffer;
		size_t capacity;
		size_t top;
		size_t high;

		LinearAllocator(const size_t cap);

		virtual ~LinearAllocator();

		void resize(const size_t cap);

		virtual void reset();

		virtual void* alloc(const size_t size);

		virtual void free(void* /*ptr*/);
	};

	struct FastLZCompressor : public dtTileCacheCompressor
	{
		virtual ~FastLZCompressor();

		virtual int maxCompressedSize(const int bufferSize);

		virtual dtStatus compress(const unsigned char* buffer, const int bufferSize,
			unsigned char* compressed, const int /*maxCompressedSize*/, int* compressedSize);

		virtual dtStatus decompress(const unsigned char* compressed, const int compressedSize,
			unsigned char* buffer, const int maxBufferSize, int* bufferSize);

	};

	struct MeshProcess : public dtTileCacheMeshProcess
	{
		InputGeom* m_geom;

		MeshProcess();

		virtual ~MeshProcess();

		void init(InputGeom* geom);

		virtual void process(struct dtNavMeshCreateParams* params, unsigned char* polyAreas, unsigned short* polyFlags);
	};

	struct TileCacheData
	{
		unsigned char* data;
		int dataSize;
	};

	struct RasterizationContext
	{
		RasterizationContext();

		~RasterizationContext();

		rcHeightfield* solid;
		unsigned char* triareas;
		rcHeightfieldLayerSet* lset;
		rcCompactHeightfield* chf;
		TileCacheData tiles[MAX_LAYERS];
		int ntiles;
	};

	struct NavMeshSetHeader
	{
		int magic;
		int version;
		int numTiles;
		dtNavMeshParams params;
	};

	struct NavMeshTileHeader
	{
		dtTileRef tileRef;
		int dataSize;
	};

	struct TileCacheSetHeader
	{
		int magic;
		int version;
		int numTiles;
		dtNavMeshParams meshParams;
		dtTileCacheParams cacheParams;
	};

	struct TileCacheTileHeader
	{
		dtCompressedTileRef tileRef;
		int dataSize;
	};

	struct Test
	{
		Test() :
			type(),
			spos(),
			epos(),
			nspos(),
			nepos(),
			radius(0),
			includeFlags(0),
			excludeFlags(0),
			expand(false),
			straight(0),
			nstraight(0),
			polys(0),
			npolys(0),
			findNearestPolyTime(0),
			findPathTime(0),
			findStraightPathTime(0),
			next(0)
		{
		}

		~Test()
		{
			delete[] straight;
			delete[] polys;
		}

		TestType type;
		float spos[3];
		float epos[3];
		float nspos[3];
		float nepos[3];
		float radius;
		unsigned short includeFlags;
		unsigned short excludeFlags;
		bool expand;

		float* straight;
		int nstraight;
		dtPolyRef* polys;
		int npolys;

		int findNearestPolyTime;
		int findPathTime;
		int findStraightPathTime;

		Test* next;
	private:
		// Explicitly disabled copy constructor and copy assignment operator.
		Test(const Test&);
		Test& operator=(const Test&);
	};
}
