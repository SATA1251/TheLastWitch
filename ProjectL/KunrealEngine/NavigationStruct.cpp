#include "NavigationStruct.h"
#include "DetourCommon.h"
#include "RecastDump.h"

namespace KunrealEngine
{
	/// LinearAllocator
	LinearAllocator::LinearAllocator(const size_t cap)
		: buffer(0), capacity(0), top(0), high(0)
	{
		resize(cap);
	}

	LinearAllocator::~LinearAllocator()
	{
		// Defined out of line to fix the weak v-tables warning
		dtFree(buffer);
	}

	void LinearAllocator::resize(const size_t cap)
	{
		if (buffer) dtFree(buffer);
		buffer = (unsigned char*)dtAlloc(cap, DT_ALLOC_PERM);
		capacity = cap;
	}

	void LinearAllocator::reset()
	{
		high = dtMax(high, top);
		top = 0;
	}

	void* LinearAllocator::alloc(const size_t size)
	{
		if (!buffer)
			return 0;
		if (top + size > capacity)
			return 0;
		unsigned char* mem = &buffer[top];
		top += size;
		return mem;
	}

	void LinearAllocator::free(void*)
	{
		// Empty
	}
	/// LinearAllocator

	/// FastLZCompressor
	FastLZCompressor::~FastLZCompressor()
	{
		// Defined out of line to fix the weak v-tables warning
	}

	int FastLZCompressor::maxCompressedSize(const int bufferSize)
	{
		return (int)(bufferSize * 1.05f);
	}

	dtStatus FastLZCompressor::compress(const unsigned char* buffer, const int bufferSize, unsigned char* compressed, const int, int* compressedSize)
	{
		*compressedSize = fastlz_compress((const void* const)buffer, bufferSize, compressed);
		return DT_SUCCESS;
	}

	dtStatus FastLZCompressor::decompress(const unsigned char* compressed, const int compressedSize, unsigned char* buffer, const int maxBufferSize, int* bufferSize)
	{
		*bufferSize = fastlz_decompress(compressed, compressedSize, buffer, maxBufferSize);
		return *bufferSize < 0 ? DT_FAILURE : DT_SUCCESS;
	}
	/// FastLZCompressor

	/// MeshProcess
	MeshProcess::MeshProcess()
		: m_geom(0)
	{

	}

	MeshProcess::~MeshProcess()
	{
	}

	void MeshProcess::init(InputGeom* geom)
	{
		m_geom = geom;
	}

	void MeshProcess::process(struct dtNavMeshCreateParams* params,
		unsigned char* polyAreas, unsigned short* polyFlags)
	{
		// Update poly flags from areas.
		for (int i = 0; i < params->polyCount; ++i)
		{
			if (polyAreas[i] == DT_TILECACHE_WALKABLE_AREA)
				polyAreas[i] = SAMPLE_POLYAREA_GROUND;

			if (polyAreas[i] == SAMPLE_POLYAREA_GROUND ||
				polyAreas[i] == SAMPLE_POLYAREA_GRASS ||
				polyAreas[i] == SAMPLE_POLYAREA_ROAD)
			{
				polyFlags[i] = SAMPLE_POLYFLAGS_WALK;
			}
			else if (polyAreas[i] == SAMPLE_POLYAREA_WATER)
			{
				polyFlags[i] = SAMPLE_POLYFLAGS_SWIM;
			}
			else if (polyAreas[i] == SAMPLE_POLYAREA_DOOR)
			{
				polyFlags[i] = SAMPLE_POLYFLAGS_WALK | SAMPLE_POLYFLAGS_DOOR;
			}
		}

		// Pass in off-mesh connections.
		if (m_geom)
		{
			params->offMeshConVerts = m_geom->getOffMeshConnectionVerts();
			params->offMeshConRad = m_geom->getOffMeshConnectionRads();
			params->offMeshConDir = m_geom->getOffMeshConnectionDirs();
			params->offMeshConAreas = m_geom->getOffMeshConnectionAreas();
			params->offMeshConFlags = m_geom->getOffMeshConnectionFlags();
			params->offMeshConUserID = m_geom->getOffMeshConnectionId();
			params->offMeshConCount = m_geom->getOffMeshConnectionCount();
		}
	}
	/// MeshProcess

	/// RasterizationContext
	RasterizationContext::RasterizationContext()
		:
		solid(0),
		triareas(0),
		lset(0),
		chf(0),
		ntiles(0)
	{
		memset(tiles, 0, sizeof(TileCacheData) * MAX_LAYERS);
	}
	RasterizationContext::~RasterizationContext()
	{
		rcFreeHeightField(solid);
		delete[] triareas;
		rcFreeHeightfieldLayerSet(lset);
		rcFreeCompactHeightfield(chf);
		for (int i = 0; i < MAX_LAYERS; ++i)
		{
			dtFree(tiles[i].data);
			tiles[i].data = 0;
		}
	}
	/// RasterizationContext
}